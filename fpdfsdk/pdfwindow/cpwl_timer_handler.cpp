// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/pdfwindow/cpwl_timer_handler.h"

#include "fpdfsdk/pdfwindow/cpwl_timer.h"
#include "third_party/base/ptr_util.h"

CPWL_TimerHandler::CPWL_TimerHandler() {}

CPWL_TimerHandler::~CPWL_TimerHandler() {}

void CPWL_TimerHandler::BeginTimer(int32_t nElapse) {
  if (!m_pTimer)
    m_pTimer = pdfium::MakeUnique<CPWL_Timer>(this, GetSystemHandler());
  m_pTimer->SetPWLTimer(nElapse);
}

void CPWL_TimerHandler::EndTimer() {
  if (m_pTimer)
    m_pTimer->KillPWLTimer();
}

void CPWL_TimerHandler::TimerProc() {}
