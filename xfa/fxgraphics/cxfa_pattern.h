// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXGRAPHICS_CXFA_PATTERN_H_
#define XFA_FXGRAPHICS_CXFA_PATTERN_H_

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_system.h"
#include "xfa/fxgraphics/cxfa_graphics.h"

class CFX_DIBitmap;
class CFX_Matrix;

class CXFA_Pattern {
 public:
  CXFA_Pattern(FX_HatchStyle hatchStyle,
               const FX_ARGB foreArgb,
               const FX_ARGB backArgb,
               CFX_Matrix* matrix = nullptr);

  virtual ~CXFA_Pattern();

 private:
  friend class CXFA_Graphics;

  CFX_Matrix m_matrix;

  const FX_HatchStyle m_hatchStyle;
  const FX_ARGB m_foreArgb;
  const FX_ARGB m_backArgb;
};

#endif  // XFA_FXGRAPHICS_CXFA_PATTERN_H_
