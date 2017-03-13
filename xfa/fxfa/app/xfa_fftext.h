// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_APP_XFA_FFTEXT_H_
#define XFA_FXFA_APP_XFA_FFTEXT_H_

#include "xfa/fxfa/app/xfa_ffdraw.h"

class CXFA_FFText : public CXFA_FFDraw {
 public:
  explicit CXFA_FFText(CXFA_WidgetAcc* pDataAcc);
  ~CXFA_FFText() override;

  // CXFA_FFWidget
  bool OnLButtonDown(uint32_t dwFlags, const CFX_PointF& point) override;
  bool OnLButtonUp(uint32_t dwFlags, const CFX_PointF& point) override;
  bool OnMouseMove(uint32_t dwFlags, const CFX_PointF& point) override;
  FWL_WidgetHit OnHitTest(const CFX_PointF& point) override;
  void RenderWidget(CFX_Graphics* pGS,
                    CFX_Matrix* pMatrix,
                    uint32_t dwStatus) override;
  bool IsLoaded() override;
  bool PerformLayout() override;

 private:
  const wchar_t* GetLinkURLAtPoint(const CFX_PointF& point);
};

#endif  // XFA_FXFA_APP_XFA_FFTEXT_H_
