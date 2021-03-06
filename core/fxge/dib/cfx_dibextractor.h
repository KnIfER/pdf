// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_DIB_CFX_DIBEXTRACTOR_H_
#define CORE_FXGE_DIB_CFX_DIBEXTRACTOR_H_

#include "core/fxcrt/cfx_retain_ptr.h"
#include "core/fxge/dib/cfx_dibitmap.h"

class CFX_DIBSource;

class CFX_DIBExtractor {
 public:
  explicit CFX_DIBExtractor(const CFX_RetainPtr<CFX_DIBSource>& pSrc);
  ~CFX_DIBExtractor();

  CFX_RetainPtr<CFX_DIBitmap> GetBitmap() { return m_pBitmap; }

 private:
  CFX_RetainPtr<CFX_DIBitmap> m_pBitmap;
};

#endif  // CORE_FXGE_DIB_CFX_DIBEXTRACTOR_H_
