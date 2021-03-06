// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_PDFWINDOW_CPWL_UTILS_H_
#define FPDFSDK_PDFWINDOW_CPWL_UTILS_H_

#include "core/fpdfdoc/cpvt_wordrange.h"
#include "fpdfsdk/pdfwindow/cpwl_wnd.h"

class CFX_Edit;
struct CFX_Color;

// checkbox & radiobutton style
#define PCS_CHECK 0
#define PCS_CIRCLE 1
#define PCS_CROSS 2
#define PCS_DIAMOND 3
#define PCS_SQUARE 4
#define PCS_STAR 5

// pushbutton layout style
#define PPBL_LABEL 0
#define PPBL_ICON 1
#define PPBL_ICONTOPLABELBOTTOM 2
#define PPBL_LABELTOPICONBOTTOM 3
#define PPBL_ICONLEFTLABELRIGHT 4
#define PPBL_LABELLEFTICONRIGHT 5
#define PPBL_LABELOVERICON 6

class CPWL_Utils {
 public:
  static CFX_ByteString GetColorAppStream(const CFX_Color& color,
                                          const bool& bFillOrStroke = true);
  static CFX_ByteString GetBorderAppStream(const CFX_FloatRect& rect,
                                           float fWidth,
                                           const CFX_Color& color,
                                           const CFX_Color& crLeftTop,
                                           const CFX_Color& crRightBottom,
                                           BorderStyle nStyle,
                                           const CPWL_Dash& dash);
  static CFX_ByteString GetCircleBorderAppStream(const CFX_FloatRect& rect,
                                                 float fWidth,
                                                 const CFX_Color& color,
                                                 const CFX_Color& crLeftTop,
                                                 const CFX_Color& crRightBottom,
                                                 BorderStyle nStyle,
                                                 const CPWL_Dash& dash);
  static CFX_ByteString GetRectFillAppStream(const CFX_FloatRect& rect,
                                             const CFX_Color& color);
  static CFX_ByteString GetCircleFillAppStream(const CFX_FloatRect& rect,
                                               const CFX_Color& color);
  static CFX_ByteString GetPushButtonAppStream(const CFX_FloatRect& rcBBox,
                                               IPVT_FontMap* pFontMap,
                                               CPDF_Stream* pIconStream,
                                               CPDF_IconFit& IconFit,
                                               const CFX_WideString& sLabel,
                                               const CFX_Color& crText,
                                               float fFontSize,
                                               int32_t nLayOut);
  static CFX_ByteString GetCheckBoxAppStream(const CFX_FloatRect& rcBBox,
                                             int32_t nStyle,
                                             const CFX_Color& crText);
  static CFX_ByteString GetRadioButtonAppStream(const CFX_FloatRect& rcBBox,
                                                int32_t nStyle,
                                                const CFX_Color& crText);
  static CFX_ByteString GetEditAppStream(CFX_Edit* pEdit,
                                         const CFX_PointF& ptOffset,
                                         const CPVT_WordRange* pRange = nullptr,
                                         bool bContinuous = true,
                                         uint16_t SubWord = 0);
  static CFX_ByteString GetEditSelAppStream(
      CFX_Edit* pEdit,
      const CFX_PointF& ptOffset,
      const CPVT_WordRange* pRange = nullptr);
  static CFX_ByteString GetDropButtonAppStream(const CFX_FloatRect& rcBBox);

  static void DrawFillRect(CFX_RenderDevice* pDevice,
                           CFX_Matrix* pUser2Device,
                           const CFX_FloatRect& rect,
                           const CFX_Color& color,
                           int32_t nTransparency);
  static void DrawFillRect(CFX_RenderDevice* pDevice,
                           CFX_Matrix* pUser2Device,
                           const CFX_FloatRect& rect,
                           const FX_COLORREF& color);
  static void DrawStrokeRect(CFX_RenderDevice* pDevice,
                             CFX_Matrix* pUser2Device,
                             const CFX_FloatRect& rect,
                             const FX_COLORREF& color,
                             float fWidth);
  static void DrawStrokeLine(CFX_RenderDevice* pDevice,
                             CFX_Matrix* pUser2Device,
                             const CFX_PointF& ptMoveTo,
                             const CFX_PointF& ptLineTo,
                             const FX_COLORREF& color,
                             float fWidth);
  static void DrawBorder(CFX_RenderDevice* pDevice,
                         CFX_Matrix* pUser2Device,
                         const CFX_FloatRect& rect,
                         float fWidth,
                         const CFX_Color& color,
                         const CFX_Color& crLeftTop,
                         const CFX_Color& crRightBottom,
                         BorderStyle nStyle,
                         int32_t nTransparency);
  static void DrawFillArea(CFX_RenderDevice* pDevice,
                           CFX_Matrix* pUser2Device,
                           const CFX_PointF* pPts,
                           int32_t nCount,
                           const FX_COLORREF& color);
  static void DrawShadow(CFX_RenderDevice* pDevice,
                         CFX_Matrix* pUser2Device,
                         bool bVertical,
                         bool bHorizontal,
                         CFX_FloatRect rect,
                         int32_t nTransparency,
                         int32_t nStartGray,
                         int32_t nEndGray);

 private:
  static CFX_ByteString GetAppStream_Check(const CFX_FloatRect& rcBBox,
                                           const CFX_Color& crText);
  static CFX_ByteString GetAppStream_Circle(const CFX_FloatRect& rcBBox,
                                            const CFX_Color& crText);
  static CFX_ByteString GetAppStream_Cross(const CFX_FloatRect& rcBBox,
                                           const CFX_Color& crText);
  static CFX_ByteString GetAppStream_Diamond(const CFX_FloatRect& rcBBox,
                                             const CFX_Color& crText);
  static CFX_ByteString GetAppStream_Square(const CFX_FloatRect& rcBBox,
                                            const CFX_Color& crText);
  static CFX_ByteString GetAppStream_Star(const CFX_FloatRect& rcBBox,
                                          const CFX_Color& crText);

  static CFX_ByteString GetAP_Check(const CFX_FloatRect& crBBox);
  static CFX_ByteString GetAP_Circle(const CFX_FloatRect& crBBox);
  static CFX_ByteString GetAP_Cross(const CFX_FloatRect& crBBox);
  static CFX_ByteString GetAP_Diamond(const CFX_FloatRect& crBBox);
  static CFX_ByteString GetAP_Square(const CFX_FloatRect& crBBox);
  static CFX_ByteString GetAP_Star(const CFX_FloatRect& crBBox);
  static CFX_ByteString GetAP_HalfCircle(const CFX_FloatRect& crBBox,
                                         float fRotate);
};

#endif  // FPDFSDK_PDFWINDOW_CPWL_UTILS_H_
