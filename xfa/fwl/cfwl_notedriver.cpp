// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_notedriver.h"

#include <utility>

#include "core/fxcrt/fx_ext.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"
#include "xfa/fwl/cfwl_app.h"
#include "xfa/fwl/cfwl_eventtarget.h"
#include "xfa/fwl/cfwl_form.h"
#include "xfa/fwl/cfwl_messagekey.h"
#include "xfa/fwl/cfwl_messagekillfocus.h"
#include "xfa/fwl/cfwl_messagemouse.h"
#include "xfa/fwl/cfwl_messagemousewheel.h"
#include "xfa/fwl/cfwl_messagesetfocus.h"
#include "xfa/fwl/cfwl_noteloop.h"
#include "xfa/fwl/cfwl_widgetmgr.h"

CFWL_NoteDriver::CFWL_NoteDriver()
    : m_pHover(nullptr),
      m_pFocus(nullptr),
      m_pGrab(nullptr),
      m_pNoteLoop(pdfium::MakeUnique<CFWL_NoteLoop>()) {
  PushNoteLoop(m_pNoteLoop.get());
}

CFWL_NoteDriver::~CFWL_NoteDriver() {
  ClearEventTargets(true);
}

void CFWL_NoteDriver::SendEvent(CFWL_Event* pNote) {
  if (m_eventTargets.empty())
    return;

  for (const auto& pair : m_eventTargets) {
    CFWL_EventTarget* pEventTarget = pair.second;
    if (pEventTarget && !pEventTarget->IsInvalid())
      pEventTarget->ProcessEvent(pNote);
  }
}

void CFWL_NoteDriver::RegisterEventTarget(CFWL_Widget* pListener,
                                          CFWL_Widget* pEventSource) {
  uint32_t key = pListener->GetEventKey();
  if (key == 0) {
    do {
      key = rand();
    } while (key == 0 || pdfium::ContainsKey(m_eventTargets, key));
    pListener->SetEventKey(key);
  }
  if (!m_eventTargets[key])
    m_eventTargets[key] = new CFWL_EventTarget(pListener);

  m_eventTargets[key]->SetEventSource(pEventSource);
}

void CFWL_NoteDriver::UnregisterEventTarget(CFWL_Widget* pListener) {
  uint32_t key = pListener->GetEventKey();
  if (key == 0)
    return;

  auto it = m_eventTargets.find(key);
  if (it != m_eventTargets.end())
    it->second->FlagInvalid();
}

void CFWL_NoteDriver::PushNoteLoop(CFWL_NoteLoop* pNoteLoop) {
  m_noteLoopQueue.Add(pNoteLoop);
}

CFWL_NoteLoop* CFWL_NoteDriver::PopNoteLoop() {
  int32_t pos = m_noteLoopQueue.GetSize();
  if (pos <= 0)
    return nullptr;

  CFWL_NoteLoop* p = m_noteLoopQueue.GetAt(pos - 1);
  m_noteLoopQueue.RemoveAt(pos - 1);
  return p;
}

bool CFWL_NoteDriver::SetFocus(CFWL_Widget* pFocus) {
  if (m_pFocus == pFocus)
    return true;

  CFWL_Widget* pPrev = m_pFocus;
  m_pFocus = pFocus;
  if (pPrev) {
    if (IFWL_WidgetDelegate* pDelegate = pPrev->GetDelegate()) {
      CFWL_MessageKillFocus ms(pPrev, pPrev);
      pDelegate->OnProcessMessage(&ms);
    }
  }
  if (pFocus) {
    CFWL_Widget* pWidget =
        pFocus->GetOwnerApp()->GetWidgetMgr()->GetSystemFormWidget(pFocus);
    CFWL_Form* pForm = static_cast<CFWL_Form*>(pWidget);
    if (pForm)
      pForm->SetSubFocus(pFocus);

    if (IFWL_WidgetDelegate* pDelegate = pFocus->GetDelegate()) {
      CFWL_MessageSetFocus ms(nullptr, pFocus);
      pDelegate->OnProcessMessage(&ms);
    }
  }
  return true;
}

void CFWL_NoteDriver::Run() {
#if (_FX_OS_ == _FX_LINUX_DESKTOP_ || _FX_OS_ == _FX_WIN32_DESKTOP_ || \
     _FX_OS_ == _FX_WIN64_)
  for (;;) {
    CFWL_NoteLoop* pTopLoop = GetTopLoop();
    if (!pTopLoop || !pTopLoop->ContinueModal())
      break;
    UnqueueMessage(pTopLoop);
  }
#endif
}

void CFWL_NoteDriver::NotifyTargetHide(CFWL_Widget* pNoteTarget) {
  if (m_pFocus == pNoteTarget)
    m_pFocus = nullptr;
  if (m_pHover == pNoteTarget)
    m_pHover = nullptr;
  if (m_pGrab == pNoteTarget)
    m_pGrab = nullptr;
}

void CFWL_NoteDriver::NotifyTargetDestroy(CFWL_Widget* pNoteTarget) {
  if (m_pFocus == pNoteTarget)
    m_pFocus = nullptr;
  if (m_pHover == pNoteTarget)
    m_pHover = nullptr;
  if (m_pGrab == pNoteTarget)
    m_pGrab = nullptr;

  UnregisterEventTarget(pNoteTarget);

  for (int32_t nIndex = 0; nIndex < m_forms.GetSize(); nIndex++) {
    CFWL_Form* pForm = static_cast<CFWL_Form*>(m_forms[nIndex]);
    if (!pForm)
      continue;

    CFWL_Widget* pSubFocus = pForm->GetSubFocus();
    if (!pSubFocus)
      return;
    if (pSubFocus == pNoteTarget)
      pForm->SetSubFocus(nullptr);
  }
}

void CFWL_NoteDriver::RegisterForm(CFWL_Widget* pForm) {
  if (!pForm || m_forms.Find(pForm) >= 0)
    return;

  m_forms.Add(pForm);
  if (m_forms.GetSize() != 1)
    return;

  CFWL_NoteLoop* pLoop = m_noteLoopQueue.GetAt(0);
  if (!pLoop)
    return;

  pLoop->SetMainForm(pForm);
}

void CFWL_NoteDriver::UnRegisterForm(CFWL_Widget* pForm) {
  if (!pForm)
    return;

  int32_t nIndex = m_forms.Find(pForm);
  if (nIndex < 0)
    return;

  m_forms.RemoveAt(nIndex);
}

void CFWL_NoteDriver::QueueMessage(std::unique_ptr<CFWL_Message> pMessage) {
  m_noteQueue.push_back(std::move(pMessage));
}

void CFWL_NoteDriver::UnqueueMessage(CFWL_NoteLoop* pNoteLoop) {
  if (m_noteQueue.empty())
    return;

  std::unique_ptr<CFWL_Message> pMessage = std::move(m_noteQueue[0]);
  m_noteQueue.pop_front();

  if (!IsValidMessage(pMessage.get()))
    return;

  ProcessMessage(pMessage.get());
}

CFWL_NoteLoop* CFWL_NoteDriver::GetTopLoop() const {
  int32_t size = m_noteLoopQueue.GetSize();
  if (size <= 0)
    return nullptr;
  return m_noteLoopQueue[size - 1];
}

void CFWL_NoteDriver::ProcessMessage(CFWL_Message* pMessage) {
  CFWL_WidgetMgr* pWidgetMgr =
      pMessage->m_pDstTarget->GetOwnerApp()->GetWidgetMgr();
  CFWL_Widget* pMessageForm = pWidgetMgr->IsFormDisabled()
                                  ? pMessage->m_pDstTarget
                                  : GetMessageForm(pMessage->m_pDstTarget);
  if (!pMessageForm)
    return;
  if (!DispatchMessage(pMessage, pMessageForm))
    return;

  if (pMessage->GetType() == CFWL_Message::Type::Mouse)
    MouseSecondary(pMessage);
}

bool CFWL_NoteDriver::DispatchMessage(CFWL_Message* pMessage,
                                      CFWL_Widget* pMessageForm) {
  switch (pMessage->GetType()) {
    case CFWL_Message::Type::SetFocus: {
      if (!DoSetFocus(pMessage, pMessageForm))
        return false;
      break;
    }
    case CFWL_Message::Type::KillFocus: {
      if (!DoKillFocus(pMessage, pMessageForm))
        return false;
      break;
    }
    case CFWL_Message::Type::Key: {
      if (!DoKey(pMessage, pMessageForm))
        return false;
      break;
    }
    case CFWL_Message::Type::Mouse: {
      if (!DoMouse(pMessage, pMessageForm))
        return false;
      break;
    }
    case CFWL_Message::Type::MouseWheel: {
      if (!DoWheel(pMessage, pMessageForm))
        return false;
      break;
    }
    default:
      break;
  }
  if (IFWL_WidgetDelegate* pDelegate = pMessage->m_pDstTarget->GetDelegate())
    pDelegate->OnProcessMessage(pMessage);

  return true;
}

bool CFWL_NoteDriver::DoSetFocus(CFWL_Message* pMessage,
                                 CFWL_Widget* pMessageForm) {
  CFWL_WidgetMgr* pWidgetMgr = pMessageForm->GetOwnerApp()->GetWidgetMgr();
  if (pWidgetMgr->IsFormDisabled()) {
    m_pFocus = pMessage->m_pDstTarget;
    return true;
  }

  CFWL_Widget* pWidget = pMessage->m_pDstTarget;
  if (!pWidget)
    return false;

  CFWL_Form* pForm = static_cast<CFWL_Form*>(pWidget);
  CFWL_Widget* pSubFocus = pForm->GetSubFocus();
  if (pSubFocus && ((pSubFocus->GetStates() & FWL_WGTSTATE_Focused) == 0)) {
    pMessage->m_pDstTarget = pSubFocus;
    if (m_pFocus != pMessage->m_pDstTarget) {
      m_pFocus = pMessage->m_pDstTarget;
      return true;
    }
  }
  return false;
}

bool CFWL_NoteDriver::DoKillFocus(CFWL_Message* pMessage,
                                  CFWL_Widget* pMessageForm) {
  CFWL_WidgetMgr* pWidgetMgr = pMessageForm->GetOwnerApp()->GetWidgetMgr();
  if (pWidgetMgr->IsFormDisabled()) {
    if (m_pFocus == pMessage->m_pDstTarget)
      m_pFocus = nullptr;
    return true;
  }

  CFWL_Form* pForm = static_cast<CFWL_Form*>(pMessage->m_pDstTarget);
  if (!pForm)
    return false;

  CFWL_Widget* pSubFocus = pForm->GetSubFocus();
  if (pSubFocus && (pSubFocus->GetStates() & FWL_WGTSTATE_Focused)) {
    pMessage->m_pDstTarget = pSubFocus;
    if (m_pFocus == pMessage->m_pDstTarget) {
      m_pFocus = nullptr;
      return true;
    }
  }
  return false;
}

bool CFWL_NoteDriver::DoKey(CFWL_Message* pMessage, CFWL_Widget* pMessageForm) {
  CFWL_MessageKey* pMsg = static_cast<CFWL_MessageKey*>(pMessage);
#if (_FX_OS_ != _FX_MACOSX_)
  if (pMsg->m_dwCmd == FWL_KeyCommand::KeyDown &&
      pMsg->m_dwKeyCode == FWL_VKEY_Tab) {
    CFWL_WidgetMgr* pWidgetMgr = pMessageForm->GetOwnerApp()->GetWidgetMgr();
    CFWL_Widget* pForm = GetMessageForm(pMsg->m_pDstTarget);
    CFWL_Widget* pFocus = m_pFocus;
    if (m_pFocus && pWidgetMgr->GetSystemFormWidget(m_pFocus) != pForm)
      pFocus = nullptr;

    bool bFind = false;
    CFWL_Widget* pNextTabStop = pWidgetMgr->NextTab(pForm, pFocus, bFind);
    if (!pNextTabStop) {
      bFind = false;
      pNextTabStop = pWidgetMgr->NextTab(pForm, nullptr, bFind);
    }
    if (pNextTabStop == pFocus)
      return true;
    if (pNextTabStop)
      SetFocus(pNextTabStop);
    return true;
  }
#endif

  if (!m_pFocus) {
    if (pMsg->m_dwCmd == FWL_KeyCommand::KeyDown &&
        pMsg->m_dwKeyCode == FWL_VKEY_Return) {
      CFWL_WidgetMgr* pWidgetMgr = pMessageForm->GetOwnerApp()->GetWidgetMgr();
      CFWL_Widget* defButton = pWidgetMgr->GetDefaultButton(pMessageForm);
      if (defButton) {
        pMsg->m_pDstTarget = defButton;
        return true;
      }
    }
    return false;
  }
  pMsg->m_pDstTarget = m_pFocus;
  return true;
}

bool CFWL_NoteDriver::DoMouse(CFWL_Message* pMessage,
                              CFWL_Widget* pMessageForm) {
  CFWL_MessageMouse* pMsg = static_cast<CFWL_MessageMouse*>(pMessage);
  if (pMsg->m_dwCmd == FWL_MouseCommand::Leave ||
      pMsg->m_dwCmd == FWL_MouseCommand::Hover ||
      pMsg->m_dwCmd == FWL_MouseCommand::Enter) {
    return !!pMsg->m_pDstTarget;
  }
  if (pMsg->m_pDstTarget != pMessageForm)
    pMsg->m_pDstTarget->TransformTo(pMessageForm, pMsg->m_fx, pMsg->m_fy);
  if (!DoMouseEx(pMsg, pMessageForm))
    pMsg->m_pDstTarget = pMessageForm;
  return true;
}

bool CFWL_NoteDriver::DoWheel(CFWL_Message* pMessage,
                              CFWL_Widget* pMessageForm) {
  CFWL_WidgetMgr* pWidgetMgr = pMessageForm->GetOwnerApp()->GetWidgetMgr();
  if (!pWidgetMgr)
    return false;

  CFWL_MessageMouseWheel* pMsg = static_cast<CFWL_MessageMouseWheel*>(pMessage);
  CFWL_Widget* pDst =
      pWidgetMgr->GetWidgetAtPoint(pMessageForm, pMsg->m_fx, pMsg->m_fy);
  if (!pDst)
    return false;

  pMessageForm->TransformTo(pDst, pMsg->m_fx, pMsg->m_fy);
  pMsg->m_pDstTarget = pDst;
  return true;
}

bool CFWL_NoteDriver::DoMouseEx(CFWL_Message* pMessage,
                                CFWL_Widget* pMessageForm) {
  CFWL_WidgetMgr* pWidgetMgr = pMessageForm->GetOwnerApp()->GetWidgetMgr();
  if (!pWidgetMgr)
    return false;
  CFWL_Widget* pTarget = nullptr;
  if (m_pGrab)
    pTarget = m_pGrab;

  CFWL_MessageMouse* pMsg = static_cast<CFWL_MessageMouse*>(pMessage);
  if (!pTarget) {
    pTarget =
        pWidgetMgr->GetWidgetAtPoint(pMessageForm, pMsg->m_fx, pMsg->m_fy);
  }
  if (pTarget) {
    if (pMessageForm != pTarget)
      pMessageForm->TransformTo(pTarget, pMsg->m_fx, pMsg->m_fy);
  }
  if (!pTarget)
    return false;

  pMsg->m_pDstTarget = pTarget;
  return true;
}

void CFWL_NoteDriver::MouseSecondary(CFWL_Message* pMessage) {
  CFWL_Widget* pTarget = pMessage->m_pDstTarget;
  if (pTarget == m_pHover)
    return;

  CFWL_MessageMouse* pMsg = static_cast<CFWL_MessageMouse*>(pMessage);
  if (m_pHover) {
    CFWL_MessageMouse msLeave(nullptr, m_pHover);
    msLeave.m_fx = pMsg->m_fx;
    msLeave.m_fy = pMsg->m_fy;
    pTarget->TransformTo(m_pHover, msLeave.m_fx, msLeave.m_fy);

    msLeave.m_dwFlags = 0;
    msLeave.m_dwCmd = FWL_MouseCommand::Leave;
    DispatchMessage(&msLeave, nullptr);
  }
  if (pTarget->GetClassID() == FWL_Type::Form) {
    m_pHover = nullptr;
    return;
  }
  m_pHover = pTarget;

  CFWL_MessageMouse msHover(nullptr, pTarget);
  msHover.m_fx = pMsg->m_fx;
  msHover.m_fy = pMsg->m_fy;
  msHover.m_dwFlags = 0;
  msHover.m_dwCmd = FWL_MouseCommand::Hover;
  DispatchMessage(&msHover, nullptr);
}

bool CFWL_NoteDriver::IsValidMessage(CFWL_Message* pMessage) {
  for (int32_t i = 0; i < m_noteLoopQueue.GetSize(); i++) {
    CFWL_NoteLoop* pNoteLoop = m_noteLoopQueue[i];
    CFWL_Widget* pForm = pNoteLoop->GetForm();
    if (pForm && (pForm == pMessage->m_pDstTarget))
      return true;
  }

  for (int32_t j = 0; j < m_forms.GetSize(); j++) {
    CFWL_Form* pForm = static_cast<CFWL_Form*>(m_forms[j]);
    if (pForm == pMessage->m_pDstTarget)
      return true;
  }
  return false;
}

CFWL_Widget* CFWL_NoteDriver::GetMessageForm(CFWL_Widget* pDstTarget) {
  int32_t iTrackLoop = m_noteLoopQueue.GetSize();
  if (iTrackLoop <= 0)
    return nullptr;

  CFWL_Widget* pMessageForm = nullptr;
  if (iTrackLoop > 1)
    pMessageForm = m_noteLoopQueue[iTrackLoop - 1]->GetForm();
  else if (m_forms.Find(pDstTarget) < 0)
    pMessageForm = pDstTarget;
  if (!pMessageForm && pDstTarget) {
    CFWL_WidgetMgr* pWidgetMgr = pDstTarget->GetOwnerApp()->GetWidgetMgr();
    if (!pWidgetMgr)
      return nullptr;
    pMessageForm = pWidgetMgr->GetSystemFormWidget(pDstTarget);
  }
  return pMessageForm;
}

void CFWL_NoteDriver::ClearEventTargets(bool bRemoveAll) {
  auto it = m_eventTargets.begin();
  while (it != m_eventTargets.end()) {
    auto old = it++;
    if (old->second && (bRemoveAll || old->second->IsInvalid())) {
      delete old->second;
      m_eventTargets.erase(old);
    }
  }
}