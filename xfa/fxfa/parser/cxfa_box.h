// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_BOX_H_
#define XFA_FXFA_PARSER_CXFA_BOX_H_

#include <vector>

#include "core/fxcrt/fx_system.h"
#include "xfa/fxfa/parser/cxfa_data.h"
#include "xfa/fxfa/parser/cxfa_edge.h"
#include "xfa/fxfa/parser/cxfa_fill.h"
#include "xfa/fxfa/parser/cxfa_margin.h"

class CXFA_Node;

class CXFA_Box : public CXFA_Data {
 public:
  explicit CXFA_Box(CXFA_Node* pNode) : CXFA_Data(pNode) {}

  bool IsArc() const { return GetElementType() == XFA_Element::Arc; }
  bool IsBorder() const { return GetElementType() == XFA_Element::Border; }
  bool IsRectangle() const {
    return GetElementType() == XFA_Element::Rectangle;
  }
  int32_t GetHand() const;
  int32_t GetPresence() const;
  int32_t CountEdges() const;
  CXFA_Edge GetEdge(int32_t nIndex = 0) const;
  void GetStrokes(std::vector<CXFA_Stroke>* strokes) const;
  bool IsCircular() const;
  bool GetStartAngle(float& fStartAngle) const;
  float GetStartAngle() const {
    float fStartAngle;
    GetStartAngle(fStartAngle);
    return fStartAngle;
  }

  bool GetSweepAngle(float& fSweepAngle) const;
  float GetSweepAngle() const {
    float fSweepAngle;
    GetSweepAngle(fSweepAngle);
    return fSweepAngle;
  }

  CXFA_Fill GetFill(bool bModified = false) const;
  CXFA_Margin GetMargin() const;
  int32_t Get3DStyle(bool& bVisible, float& fThickness) const;
};

#endif  // XFA_FXFA_PARSER_CXFA_BOX_H_
