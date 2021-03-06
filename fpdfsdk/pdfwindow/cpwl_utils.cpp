// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/pdfwindow/cpwl_utils.h"

#include <algorithm>
#include <memory>
#include <sstream>

#include "core/fpdfdoc/cpvt_word.h"
#include "core/fxge/cfx_graphstatedata.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/cfx_renderdevice.h"
#include "fpdfsdk/fxedit/fxet_edit.h"
#include "fpdfsdk/pdfwindow/cpwl_icon.h"
#include "fpdfsdk/pdfwindow/cpwl_wnd.h"

CFX_ByteString CPWL_Utils::GetAP_Check(const CFX_FloatRect& crBBox) {
  const float fWidth = crBBox.right - crBBox.left;
  const float fHeight = crBBox.top - crBBox.bottom;

  CFX_PointF pts[8][3] = {{CFX_PointF(0.28f, 0.52f), CFX_PointF(0.27f, 0.48f),
                           CFX_PointF(0.29f, 0.40f)},
                          {CFX_PointF(0.30f, 0.33f), CFX_PointF(0.31f, 0.29f),
                           CFX_PointF(0.31f, 0.28f)},
                          {CFX_PointF(0.39f, 0.28f), CFX_PointF(0.49f, 0.29f),
                           CFX_PointF(0.77f, 0.67f)},
                          {CFX_PointF(0.76f, 0.68f), CFX_PointF(0.78f, 0.69f),
                           CFX_PointF(0.76f, 0.75f)},
                          {CFX_PointF(0.76f, 0.75f), CFX_PointF(0.73f, 0.80f),
                           CFX_PointF(0.68f, 0.75f)},
                          {CFX_PointF(0.68f, 0.74f), CFX_PointF(0.68f, 0.74f),
                           CFX_PointF(0.44f, 0.47f)},
                          {CFX_PointF(0.43f, 0.47f), CFX_PointF(0.40f, 0.47f),
                           CFX_PointF(0.41f, 0.58f)},
                          {CFX_PointF(0.40f, 0.60f), CFX_PointF(0.28f, 0.66f),
                           CFX_PointF(0.30f, 0.56f)}};

  for (size_t i = 0; i < FX_ArraySize(pts); ++i) {
    for (size_t j = 0; j < FX_ArraySize(pts[0]); ++j) {
      pts[i][j].x = pts[i][j].x * fWidth + crBBox.left;
      pts[i][j].y *= pts[i][j].y * fHeight + crBBox.bottom;
    }
  }

  std::ostringstream csAP;
  csAP << pts[0][0].x << " " << pts[0][0].y << " m\n";

  for (size_t i = 0; i < FX_ArraySize(pts); ++i) {
    size_t nNext = i < FX_ArraySize(pts) - 1 ? i + 1 : 0;

    float px1 = pts[i][1].x - pts[i][0].x;
    float py1 = pts[i][1].y - pts[i][0].y;
    float px2 = pts[i][2].x - pts[nNext][0].x;
    float py2 = pts[i][2].y - pts[nNext][0].y;

    csAP << pts[i][0].x + px1 * FX_BEZIER << " "
         << pts[i][0].y + py1 * FX_BEZIER << " "
         << pts[nNext][0].x + px2 * FX_BEZIER << " "
         << pts[nNext][0].y + py2 * FX_BEZIER << " " << pts[nNext][0].x << " "
         << pts[nNext][0].y << " c\n";
  }

  return CFX_ByteString(csAP);
}

CFX_ByteString CPWL_Utils::GetAP_Circle(const CFX_FloatRect& crBBox) {
  std::ostringstream csAP;

  float fWidth = crBBox.right - crBBox.left;
  float fHeight = crBBox.top - crBBox.bottom;

  CFX_PointF pt1(crBBox.left, crBBox.bottom + fHeight / 2);
  CFX_PointF pt2(crBBox.left + fWidth / 2, crBBox.top);
  CFX_PointF pt3(crBBox.right, crBBox.bottom + fHeight / 2);
  CFX_PointF pt4(crBBox.left + fWidth / 2, crBBox.bottom);

  csAP << pt1.x << " " << pt1.y << " m\n";

  float px = pt2.x - pt1.x;
  float py = pt2.y - pt1.y;

  csAP << pt1.x << " " << pt1.y + py * FX_BEZIER << " "
       << pt2.x - px * FX_BEZIER << " " << pt2.y << " " << pt2.x << " " << pt2.y
       << " c\n";

  px = pt3.x - pt2.x;
  py = pt2.y - pt3.y;

  csAP << pt2.x + px * FX_BEZIER << " " << pt2.y << " " << pt3.x << " "
       << pt3.y + py * FX_BEZIER << " " << pt3.x << " " << pt3.y << " c\n";

  px = pt3.x - pt4.x;
  py = pt3.y - pt4.y;

  csAP << pt3.x << " " << pt3.y - py * FX_BEZIER << " "
       << pt4.x + px * FX_BEZIER << " " << pt4.y << " " << pt4.x << " " << pt4.y
       << " c\n";

  px = pt4.x - pt1.x;
  py = pt1.y - pt4.y;

  csAP << pt4.x - px * FX_BEZIER << " " << pt4.y << " " << pt1.x << " "
       << pt1.y - py * FX_BEZIER << " " << pt1.x << " " << pt1.y << " c\n";

  return CFX_ByteString(csAP);
}

CFX_ByteString CPWL_Utils::GetAP_Cross(const CFX_FloatRect& crBBox) {
  std::ostringstream csAP;

  csAP << crBBox.left << " " << crBBox.top << " m\n";
  csAP << crBBox.right << " " << crBBox.bottom << " l\n";
  csAP << crBBox.left << " " << crBBox.bottom << " m\n";
  csAP << crBBox.right << " " << crBBox.top << " l\n";

  return CFX_ByteString(csAP);
}

CFX_ByteString CPWL_Utils::GetAP_Diamond(const CFX_FloatRect& crBBox) {
  std::ostringstream csAP;

  float fWidth = crBBox.right - crBBox.left;
  float fHeight = crBBox.top - crBBox.bottom;

  CFX_PointF pt1(crBBox.left, crBBox.bottom + fHeight / 2);
  CFX_PointF pt2(crBBox.left + fWidth / 2, crBBox.top);
  CFX_PointF pt3(crBBox.right, crBBox.bottom + fHeight / 2);
  CFX_PointF pt4(crBBox.left + fWidth / 2, crBBox.bottom);

  csAP << pt1.x << " " << pt1.y << " m\n";
  csAP << pt2.x << " " << pt2.y << " l\n";
  csAP << pt3.x << " " << pt3.y << " l\n";
  csAP << pt4.x << " " << pt4.y << " l\n";
  csAP << pt1.x << " " << pt1.y << " l\n";

  return CFX_ByteString(csAP);
}

CFX_ByteString CPWL_Utils::GetAP_Square(const CFX_FloatRect& crBBox) {
  std::ostringstream csAP;

  csAP << crBBox.left << " " << crBBox.top << " m\n";
  csAP << crBBox.right << " " << crBBox.top << " l\n";
  csAP << crBBox.right << " " << crBBox.bottom << " l\n";
  csAP << crBBox.left << " " << crBBox.bottom << " l\n";
  csAP << crBBox.left << " " << crBBox.top << " l\n";

  return CFX_ByteString(csAP);
}

CFX_ByteString CPWL_Utils::GetAP_Star(const CFX_FloatRect& crBBox) {
  std::ostringstream csAP;

  float fRadius = (crBBox.top - crBBox.bottom) / (1 + (float)cos(FX_PI / 5.0f));
  CFX_PointF ptCenter = CFX_PointF((crBBox.left + crBBox.right) / 2.0f,
                                   (crBBox.top + crBBox.bottom) / 2.0f);

  float px[5], py[5];

  float fAngel = FX_PI / 10.0f;

  for (int32_t i = 0; i < 5; i++) {
    px[i] = ptCenter.x + fRadius * (float)cos(fAngel);
    py[i] = ptCenter.y + fRadius * (float)sin(fAngel);

    fAngel += FX_PI * 2 / 5.0f;
  }

  csAP << px[0] << " " << py[0] << " m\n";

  int32_t nNext = 0;
  for (int32_t j = 0; j < 5; j++) {
    nNext += 2;
    if (nNext >= 5)
      nNext -= 5;
    csAP << px[nNext] << " " << py[nNext] << " l\n";
  }

  return CFX_ByteString(csAP);
}

CFX_ByteString CPWL_Utils::GetAP_HalfCircle(const CFX_FloatRect& crBBox,
                                            float fRotate) {
  std::ostringstream csAP;

  float fWidth = crBBox.right - crBBox.left;
  float fHeight = crBBox.top - crBBox.bottom;

  CFX_PointF pt1(-fWidth / 2, 0);
  CFX_PointF pt2(0, fHeight / 2);
  CFX_PointF pt3(fWidth / 2, 0);

  float px, py;

  csAP << cos(fRotate) << " " << sin(fRotate) << " " << -sin(fRotate) << " "
       << cos(fRotate) << " " << crBBox.left + fWidth / 2 << " "
       << crBBox.bottom + fHeight / 2 << " cm\n";

  csAP << pt1.x << " " << pt1.y << " m\n";

  px = pt2.x - pt1.x;
  py = pt2.y - pt1.y;

  csAP << pt1.x << " " << pt1.y + py * FX_BEZIER << " "
       << pt2.x - px * FX_BEZIER << " " << pt2.y << " " << pt2.x << " " << pt2.y
       << " c\n";

  px = pt3.x - pt2.x;
  py = pt2.y - pt3.y;

  csAP << pt2.x + px * FX_BEZIER << " " << pt2.y << " " << pt3.x << " "
       << pt3.y + py * FX_BEZIER << " " << pt3.x << " " << pt3.y << " c\n";

  return CFX_ByteString(csAP);
}

CFX_ByteString CPWL_Utils::GetRectFillAppStream(const CFX_FloatRect& rect,
                                                const CFX_Color& color) {
  std::ostringstream sAppStream;
  CFX_ByteString sColor = GetColorAppStream(color, true);
  if (sColor.GetLength() > 0) {
    sAppStream << "q\n" << sColor;
    sAppStream << rect.left << " " << rect.bottom << " "
               << rect.right - rect.left << " " << rect.top - rect.bottom
               << " re f\nQ\n";
  }

  return CFX_ByteString(sAppStream);
}

CFX_ByteString CPWL_Utils::GetCircleFillAppStream(const CFX_FloatRect& rect,
                                                  const CFX_Color& color) {
  std::ostringstream sAppStream;
  CFX_ByteString sColor = GetColorAppStream(color, true);
  if (sColor.GetLength() > 0)
    sAppStream << "q\n" << sColor << CPWL_Utils::GetAP_Circle(rect) << "f\nQ\n";
  return CFX_ByteString(sAppStream);
}

CFX_ByteString CPWL_Utils::GetEditAppStream(CFX_Edit* pEdit,
                                            const CFX_PointF& ptOffset,
                                            const CPVT_WordRange* pRange,
                                            bool bContinuous,
                                            uint16_t SubWord) {
  return CFX_Edit::GetEditAppearanceStream(pEdit, ptOffset, pRange, bContinuous,
                                           SubWord);
}

CFX_ByteString CPWL_Utils::GetEditSelAppStream(CFX_Edit* pEdit,
                                               const CFX_PointF& ptOffset,
                                               const CPVT_WordRange* pRange) {
  return CFX_Edit::GetSelectAppearanceStream(pEdit, ptOffset, pRange);
}

CFX_ByteString CPWL_Utils::GetPushButtonAppStream(const CFX_FloatRect& rcBBox,
                                                  IPVT_FontMap* pFontMap,
                                                  CPDF_Stream* pIconStream,
                                                  CPDF_IconFit& IconFit,
                                                  const CFX_WideString& sLabel,
                                                  const CFX_Color& crText,
                                                  float fFontSize,
                                                  int32_t nLayOut) {
  const float fAutoFontScale = 1.0f / 3.0f;

  auto pEdit = pdfium::MakeUnique<CFX_Edit>();
  pEdit->SetFontMap(pFontMap);
  pEdit->SetAlignmentH(1, true);
  pEdit->SetAlignmentV(1, true);
  pEdit->SetMultiLine(false, true);
  pEdit->SetAutoReturn(false, true);
  if (IsFloatZero(fFontSize))
    pEdit->SetAutoFontSize(true, true);
  else
    pEdit->SetFontSize(fFontSize);

  pEdit->Initialize();
  pEdit->SetText(sLabel);

  CFX_FloatRect rcLabelContent = pEdit->GetContentRect();
  CPWL_Icon Icon;
  PWL_CREATEPARAM cp;
  cp.dwFlags = PWS_VISIBLE;
  Icon.Create(cp);
  Icon.SetIconFit(&IconFit);
  Icon.SetPDFStream(pIconStream);

  CFX_FloatRect rcLabel;
  CFX_FloatRect rcIcon;
  float fWidth = 0.0f;
  float fHeight = 0.0f;

  switch (nLayOut) {
    case PPBL_LABEL:
      rcLabel = rcBBox;
      break;
    case PPBL_ICON:
      rcIcon = rcBBox;
      break;
    case PPBL_ICONTOPLABELBOTTOM:
      if (pIconStream) {
        if (IsFloatZero(fFontSize)) {
          fHeight = rcBBox.top - rcBBox.bottom;
          rcLabel = CFX_FloatRect(rcBBox.left, rcBBox.bottom, rcBBox.right,
                                  rcBBox.bottom + fHeight * fAutoFontScale);
          rcIcon =
              CFX_FloatRect(rcBBox.left, rcLabel.top, rcBBox.right, rcBBox.top);
        } else {
          fHeight = rcLabelContent.Height();

          if (rcBBox.bottom + fHeight > rcBBox.top) {
            rcLabel = rcBBox;
          } else {
            rcLabel = CFX_FloatRect(rcBBox.left, rcBBox.bottom, rcBBox.right,
                                    rcBBox.bottom + fHeight);
            rcIcon = CFX_FloatRect(rcBBox.left, rcLabel.top, rcBBox.right,
                                   rcBBox.top);
          }
        }
      } else {
        rcLabel = rcBBox;
      }
      break;
    case PPBL_LABELTOPICONBOTTOM:
      if (pIconStream) {
        if (IsFloatZero(fFontSize)) {
          fHeight = rcBBox.top - rcBBox.bottom;
          rcLabel =
              CFX_FloatRect(rcBBox.left, rcBBox.top - fHeight * fAutoFontScale,
                            rcBBox.right, rcBBox.top);
          rcIcon = CFX_FloatRect(rcBBox.left, rcBBox.bottom, rcBBox.right,
                                 rcLabel.bottom);
        } else {
          fHeight = rcLabelContent.Height();

          if (rcBBox.bottom + fHeight > rcBBox.top) {
            rcLabel = rcBBox;
          } else {
            rcLabel = CFX_FloatRect(rcBBox.left, rcBBox.top - fHeight,
                                    rcBBox.right, rcBBox.top);
            rcIcon = CFX_FloatRect(rcBBox.left, rcBBox.bottom, rcBBox.right,
                                   rcLabel.bottom);
          }
        }
      } else {
        rcLabel = rcBBox;
      }
      break;
    case PPBL_ICONLEFTLABELRIGHT:
      if (pIconStream) {
        if (IsFloatZero(fFontSize)) {
          fWidth = rcBBox.right - rcBBox.left;
          if (rcLabelContent.Width() < fWidth * fAutoFontScale) {
            rcLabel = CFX_FloatRect(rcBBox.right - fWidth * fAutoFontScale,
                                    rcBBox.bottom, rcBBox.right, rcBBox.top);
            rcIcon = CFX_FloatRect(rcBBox.left, rcBBox.bottom, rcLabel.left,
                                   rcBBox.top);
          } else {
            if (rcLabelContent.Width() < fWidth) {
              rcLabel = CFX_FloatRect(rcBBox.right - rcLabelContent.Width(),
                                      rcBBox.bottom, rcBBox.right, rcBBox.top);
              rcIcon = CFX_FloatRect(rcBBox.left, rcBBox.bottom, rcLabel.left,
                                     rcBBox.top);
            } else {
              rcLabel = rcBBox;
            }
          }
        } else {
          fWidth = rcLabelContent.Width();
          if (rcBBox.left + fWidth > rcBBox.right) {
            rcLabel = rcBBox;
          } else {
            rcLabel = CFX_FloatRect(rcBBox.right - fWidth, rcBBox.bottom,
                                    rcBBox.right, rcBBox.top);
            rcIcon = CFX_FloatRect(rcBBox.left, rcBBox.bottom, rcLabel.left,
                                   rcBBox.top);
          }
        }
      } else {
        rcLabel = rcBBox;
      }
      break;
    case PPBL_LABELLEFTICONRIGHT:
      if (pIconStream) {
        if (IsFloatZero(fFontSize)) {
          fWidth = rcBBox.right - rcBBox.left;
          if (rcLabelContent.Width() < fWidth * fAutoFontScale) {
            rcLabel = CFX_FloatRect(rcBBox.left, rcBBox.bottom,
                                    rcBBox.left + fWidth * fAutoFontScale,
                                    rcBBox.top);
            rcIcon = CFX_FloatRect(rcLabel.right, rcBBox.bottom, rcBBox.right,
                                   rcBBox.top);
          } else {
            if (rcLabelContent.Width() < fWidth) {
              rcLabel = CFX_FloatRect(rcBBox.left, rcBBox.bottom,
                                      rcBBox.left + rcLabelContent.Width(),
                                      rcBBox.top);
              rcIcon = CFX_FloatRect(rcLabel.right, rcBBox.bottom, rcBBox.right,
                                     rcBBox.top);
            } else {
              rcLabel = rcBBox;
            }
          }
        } else {
          fWidth = rcLabelContent.Width();
          if (rcBBox.left + fWidth > rcBBox.right) {
            rcLabel = rcBBox;
          } else {
            rcLabel = CFX_FloatRect(rcBBox.left, rcBBox.bottom,
                                    rcBBox.left + fWidth, rcBBox.top);
            rcIcon = CFX_FloatRect(rcLabel.right, rcBBox.bottom, rcBBox.right,
                                   rcBBox.top);
          }
        }
      } else {
        rcLabel = rcBBox;
      }
      break;
    case PPBL_LABELOVERICON:
      rcLabel = rcBBox;
      rcIcon = rcBBox;
      break;
  }

  std::ostringstream sTemp;

  if (!rcIcon.IsEmpty()) {
    Icon.Move(rcIcon, false, false);
    sTemp << Icon.GetImageAppStream();
  }

  Icon.Destroy();

  if (!rcLabel.IsEmpty()) {
    pEdit->SetPlateRect(rcLabel);
    CFX_ByteString sEdit =
        CPWL_Utils::GetEditAppStream(pEdit.get(), CFX_PointF(0.0f, 0.0f));
    if (sEdit.GetLength() > 0) {
      sTemp << "BT\n"
            << CPWL_Utils::GetColorAppStream(crText) << sEdit << "ET\n";
    }
  }

  if (sTemp.tellp() <= 0)
    return CFX_ByteString();

  std::ostringstream sAppStream;
  sAppStream << "q\n"
             << rcBBox.left << " " << rcBBox.bottom << " "
             << rcBBox.right - rcBBox.left << " " << rcBBox.top - rcBBox.bottom
             << " re W n\n";
  sAppStream << sTemp.str().c_str() << "Q\n";
  return CFX_ByteString(sAppStream);
}

CFX_ByteString CPWL_Utils::GetColorAppStream(const CFX_Color& color,
                                             const bool& bFillOrStroke) {
  std::ostringstream sColorStream;

  switch (color.nColorType) {
    case COLORTYPE_RGB:
      sColorStream << color.fColor1 << " " << color.fColor2 << " "
                   << color.fColor3 << " " << (bFillOrStroke ? "rg" : "RG")
                   << "\n";
      break;
    case COLORTYPE_GRAY:
      sColorStream << color.fColor1 << " " << (bFillOrStroke ? "g" : "G")
                   << "\n";
      break;
    case COLORTYPE_CMYK:
      sColorStream << color.fColor1 << " " << color.fColor2 << " "
                   << color.fColor3 << " " << color.fColor4 << " "
                   << (bFillOrStroke ? "k" : "K") << "\n";
      break;
  }

  return CFX_ByteString(sColorStream);
}

CFX_ByteString CPWL_Utils::GetBorderAppStream(const CFX_FloatRect& rect,
                                              float fWidth,
                                              const CFX_Color& color,
                                              const CFX_Color& crLeftTop,
                                              const CFX_Color& crRightBottom,
                                              BorderStyle nStyle,
                                              const CPWL_Dash& dash) {
  std::ostringstream sAppStream;
  CFX_ByteString sColor;

  float fLeft = rect.left;
  float fRight = rect.right;
  float fTop = rect.top;
  float fBottom = rect.bottom;

  if (fWidth > 0.0f) {
    float fHalfWidth = fWidth / 2.0f;

    sAppStream << "q\n";

    switch (nStyle) {
      default:
      case BorderStyle::SOLID:
        sColor = CPWL_Utils::GetColorAppStream(color, true);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fLeft << " " << fBottom << " " << fRight - fLeft << " "
                     << fTop - fBottom << " re\n";
          sAppStream << fLeft + fWidth << " " << fBottom + fWidth << " "
                     << fRight - fLeft - fWidth * 2 << " "
                     << fTop - fBottom - fWidth * 2 << " re\n";
          sAppStream << "f*\n";
        }
        break;
      case BorderStyle::DASH:
        sColor = CPWL_Utils::GetColorAppStream(color, false);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fWidth << " w"
                     << " [" << dash.nDash << " " << dash.nGap << "] "
                     << dash.nPhase << " d\n";
          sAppStream << fLeft + fWidth / 2 << " " << fBottom + fWidth / 2
                     << " m\n";
          sAppStream << fLeft + fWidth / 2 << " " << fTop - fWidth / 2
                     << " l\n";
          sAppStream << fRight - fWidth / 2 << " " << fTop - fWidth / 2
                     << " l\n";
          sAppStream << fRight - fWidth / 2 << " " << fBottom + fWidth / 2
                     << " l\n";
          sAppStream << fLeft + fWidth / 2 << " " << fBottom + fWidth / 2
                     << " l S\n";
        }
        break;
      case BorderStyle::BEVELED:
      case BorderStyle::INSET:
        sColor = CPWL_Utils::GetColorAppStream(crLeftTop, true);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fLeft + fHalfWidth << " " << fBottom + fHalfWidth
                     << " m\n";
          sAppStream << fLeft + fHalfWidth << " " << fTop - fHalfWidth
                     << " l\n";
          sAppStream << fRight - fHalfWidth << " " << fTop - fHalfWidth
                     << " l\n";
          sAppStream << fRight - fHalfWidth * 2 << " " << fTop - fHalfWidth * 2
                     << " l\n";
          sAppStream << fLeft + fHalfWidth * 2 << " " << fTop - fHalfWidth * 2
                     << " l\n";
          sAppStream << fLeft + fHalfWidth * 2 << " "
                     << fBottom + fHalfWidth * 2 << " l f\n";
        }

        sColor = CPWL_Utils::GetColorAppStream(crRightBottom, true);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fRight - fHalfWidth << " " << fTop - fHalfWidth
                     << " m\n";
          sAppStream << fRight - fHalfWidth << " " << fBottom + fHalfWidth
                     << " l\n";
          sAppStream << fLeft + fHalfWidth << " " << fBottom + fHalfWidth
                     << " l\n";
          sAppStream << fLeft + fHalfWidth * 2 << " "
                     << fBottom + fHalfWidth * 2 << " l\n";
          sAppStream << fRight - fHalfWidth * 2 << " "
                     << fBottom + fHalfWidth * 2 << " l\n";
          sAppStream << fRight - fHalfWidth * 2 << " " << fTop - fHalfWidth * 2
                     << " l f\n";
        }

        sColor = CPWL_Utils::GetColorAppStream(color, true);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fLeft << " " << fBottom << " " << fRight - fLeft << " "
                     << fTop - fBottom << " re\n";
          sAppStream << fLeft + fHalfWidth << " " << fBottom + fHalfWidth << " "
                     << fRight - fLeft - fHalfWidth * 2 << " "
                     << fTop - fBottom - fHalfWidth * 2 << " re f*\n";
        }
        break;
      case BorderStyle::UNDERLINE:
        sColor = CPWL_Utils::GetColorAppStream(color, false);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fWidth << " w\n";
          sAppStream << fLeft << " " << fBottom + fWidth / 2 << " m\n";
          sAppStream << fRight << " " << fBottom + fWidth / 2 << " l S\n";
        }
        break;
    }

    sAppStream << "Q\n";
  }

  return CFX_ByteString(sAppStream);
}

CFX_ByteString CPWL_Utils::GetCircleBorderAppStream(
    const CFX_FloatRect& rect,
    float fWidth,
    const CFX_Color& color,
    const CFX_Color& crLeftTop,
    const CFX_Color& crRightBottom,
    BorderStyle nStyle,
    const CPWL_Dash& dash) {
  std::ostringstream sAppStream;
  CFX_ByteString sColor;

  if (fWidth > 0.0f) {
    sAppStream << "q\n";

    float fHalfWidth = fWidth / 2.0f;

    CFX_FloatRect rect_by_2 = rect;
    CFX_FloatRect rect_by_75 = rect;
    if (!rect.IsEmpty()) {
      float div = fWidth / 2.0f;
      rect_by_2.Deflate(div, div);
      rect_by_2.Normalize();

      div = fHalfWidth * 0.75f;
      rect_by_75.Deflate(div, div);
      rect_by_75.Normalize();
    }

    switch (nStyle) {
      default:
      case BorderStyle::SOLID:
      case BorderStyle::UNDERLINE: {
        sColor = CPWL_Utils::GetColorAppStream(color, false);
        if (sColor.GetLength() > 0) {
          sAppStream << "q\n"
                     << fWidth << " w\n"
                     << sColor << CPWL_Utils::GetAP_Circle(rect_by_2)
                     << " S\nQ\n";
        }
      } break;
      case BorderStyle::DASH: {
        sColor = CPWL_Utils::GetColorAppStream(color, false);
        if (sColor.GetLength() > 0) {
          sAppStream << "q\n"
                     << fWidth << " w\n"
                     << "[" << dash.nDash << " " << dash.nGap << "] "
                     << dash.nPhase << " d\n"
                     << sColor << CPWL_Utils::GetAP_Circle(rect_by_2)
                     << " S\nQ\n";
        }
      } break;
      case BorderStyle::BEVELED: {
        sColor = CPWL_Utils::GetColorAppStream(color, false);
        if (sColor.GetLength() > 0) {
          sAppStream << "q\n"
                     << fHalfWidth << " w\n"
                     << sColor << CPWL_Utils::GetAP_Circle(rect) << " S\nQ\n";
        }

        sColor = CPWL_Utils::GetColorAppStream(crLeftTop, false);
        if (sColor.GetLength() > 0) {
          sAppStream << "q\n"
                     << fHalfWidth << " w\n"
                     << sColor
                     << CPWL_Utils::GetAP_HalfCircle(rect_by_75, FX_PI / 4.0f)
                     << " S\nQ\n";
        }

        sColor = CPWL_Utils::GetColorAppStream(crRightBottom, false);
        if (sColor.GetLength() > 0) {
          sAppStream << "q\n"
                     << fHalfWidth << " w\n"
                     << sColor
                     << CPWL_Utils::GetAP_HalfCircle(rect_by_75,
                                                     FX_PI * 5 / 4.0f)
                     << " S\nQ\n";
        }
      } break;
      case BorderStyle::INSET: {
        sColor = CPWL_Utils::GetColorAppStream(color, false);
        if (sColor.GetLength() > 0) {
          sAppStream << "q\n"
                     << fHalfWidth << " w\n"
                     << sColor << CPWL_Utils::GetAP_Circle(rect) << " S\nQ\n";
        }

        sColor = CPWL_Utils::GetColorAppStream(crLeftTop, false);
        if (sColor.GetLength() > 0) {
          sAppStream << "q\n"
                     << fHalfWidth << " w\n"
                     << sColor
                     << CPWL_Utils::GetAP_HalfCircle(rect_by_75, FX_PI / 4.0f)
                     << " S\nQ\n";
        }

        sColor = CPWL_Utils::GetColorAppStream(crRightBottom, false);
        if (sColor.GetLength() > 0) {
          sAppStream << "q\n"
                     << fHalfWidth << " w\n"
                     << sColor
                     << CPWL_Utils::GetAP_HalfCircle(rect_by_75,
                                                     FX_PI * 5 / 4.0f)
                     << " S\nQ\n";
        }
      } break;
    }

    sAppStream << "Q\n";
  }

  return CFX_ByteString(sAppStream);
}

CFX_ByteString CPWL_Utils::GetAppStream_Check(const CFX_FloatRect& rcBBox,
                                              const CFX_Color& crText) {
  std::ostringstream sAP;
  sAP << "q\n"
      << CPWL_Utils::GetColorAppStream(crText, true)
      << CPWL_Utils::GetAP_Check(rcBBox) << "f\nQ\n";
  return CFX_ByteString(sAP);
}

CFX_ByteString CPWL_Utils::GetAppStream_Circle(const CFX_FloatRect& rcBBox,
                                               const CFX_Color& crText) {
  std::ostringstream sAP;
  sAP << "q\n"
      << CPWL_Utils::GetColorAppStream(crText, true)
      << CPWL_Utils::GetAP_Circle(rcBBox) << "f\nQ\n";
  return CFX_ByteString(sAP);
}

CFX_ByteString CPWL_Utils::GetAppStream_Cross(const CFX_FloatRect& rcBBox,
                                              const CFX_Color& crText) {
  std::ostringstream sAP;
  sAP << "q\n"
      << CPWL_Utils::GetColorAppStream(crText, false)
      << CPWL_Utils::GetAP_Cross(rcBBox) << "S\nQ\n";
  return CFX_ByteString(sAP);
}

CFX_ByteString CPWL_Utils::GetAppStream_Diamond(const CFX_FloatRect& rcBBox,
                                                const CFX_Color& crText) {
  std::ostringstream sAP;
  sAP << "q\n1 w\n"
      << CPWL_Utils::GetColorAppStream(crText, true)
      << CPWL_Utils::GetAP_Diamond(rcBBox) << "f\nQ\n";
  return CFX_ByteString(sAP);
}

CFX_ByteString CPWL_Utils::GetAppStream_Square(const CFX_FloatRect& rcBBox,
                                               const CFX_Color& crText) {
  std::ostringstream sAP;
  sAP << "q\n"
      << CPWL_Utils::GetColorAppStream(crText, true)
      << CPWL_Utils::GetAP_Square(rcBBox) << "f\nQ\n";
  return CFX_ByteString(sAP);
}

CFX_ByteString CPWL_Utils::GetAppStream_Star(const CFX_FloatRect& rcBBox,
                                             const CFX_Color& crText) {
  std::ostringstream sAP;
  sAP << "q\n"
      << CPWL_Utils::GetColorAppStream(crText, true)
      << CPWL_Utils::GetAP_Star(rcBBox) << "f\nQ\n";
  return CFX_ByteString(sAP);
}

CFX_ByteString CPWL_Utils::GetCheckBoxAppStream(const CFX_FloatRect& rcBBox,
                                                int32_t nStyle,
                                                const CFX_Color& crText) {
  CFX_FloatRect rcCenter = rcBBox.GetCenterSquare();
  switch (nStyle) {
    default:
    case PCS_CHECK:
      return GetAppStream_Check(rcCenter, crText);
    case PCS_CIRCLE:
      rcCenter.Scale(2.0f / 3.0f);
      return GetAppStream_Circle(rcCenter, crText);
    case PCS_CROSS:
      return GetAppStream_Cross(rcCenter, crText);
    case PCS_DIAMOND:
      rcCenter.Scale(2.0f / 3.0f);
      return GetAppStream_Diamond(rcCenter, crText);
    case PCS_SQUARE:
      rcCenter.Scale(2.0f / 3.0f);
      return GetAppStream_Square(rcCenter, crText);
    case PCS_STAR:
      rcCenter.Scale(2.0f / 3.0f);
      return GetAppStream_Star(rcCenter, crText);
  }
}

CFX_ByteString CPWL_Utils::GetRadioButtonAppStream(const CFX_FloatRect& rcBBox,
                                                   int32_t nStyle,
                                                   const CFX_Color& crText) {
  CFX_FloatRect rcCenter = rcBBox.GetCenterSquare();
  switch (nStyle) {
    default:
    case PCS_CHECK:
      return GetAppStream_Check(rcCenter, crText);
    case PCS_CIRCLE:
      rcCenter.Scale(1.0f / 2.0f);
      return GetAppStream_Circle(rcCenter, crText);
    case PCS_CROSS:
      return GetAppStream_Cross(rcCenter, crText);
    case PCS_DIAMOND:
      rcCenter.Scale(2.0f / 3.0f);
      return GetAppStream_Diamond(rcCenter, crText);
    case PCS_SQUARE:
      rcCenter.Scale(2.0f / 3.0f);
      return GetAppStream_Square(rcCenter, crText);
    case PCS_STAR:
      rcCenter.Scale(2.0f / 3.0f);
      return GetAppStream_Star(rcCenter, crText);
  }
}

CFX_ByteString CPWL_Utils::GetDropButtonAppStream(const CFX_FloatRect& rcBBox) {
  if (rcBBox.IsEmpty())
    return CFX_ByteString();

  std::ostringstream sAppStream;
  sAppStream << "q\n"
             << CPWL_Utils::GetColorAppStream(
                    CFX_Color(COLORTYPE_RGB, 220.0f / 255.0f, 220.0f / 255.0f,
                              220.0f / 255.0f),
                    true)
             << rcBBox.left << " " << rcBBox.bottom << " "
             << rcBBox.right - rcBBox.left << " " << rcBBox.top - rcBBox.bottom
             << " re f\n"
             << "Q\n";

  sAppStream << "q\n"
             << CPWL_Utils::GetBorderAppStream(
                    rcBBox, 2, CFX_Color(COLORTYPE_GRAY, 0),
                    CFX_Color(COLORTYPE_GRAY, 1),
                    CFX_Color(COLORTYPE_GRAY, 0.5), BorderStyle::BEVELED,
                    CPWL_Dash(3, 0, 0))
             << "Q\n";

  CFX_PointF ptCenter = CFX_PointF((rcBBox.left + rcBBox.right) / 2,
                                   (rcBBox.top + rcBBox.bottom) / 2);
  if (IsFloatBigger(rcBBox.right - rcBBox.left, 6) &&
      IsFloatBigger(rcBBox.top - rcBBox.bottom, 6)) {
    sAppStream << "q\n"
               << " 0 g\n"
               << ptCenter.x - 3 << " " << ptCenter.y + 1.5f << " m\n"
               << ptCenter.x + 3 << " " << ptCenter.y + 1.5f << " l\n"
               << ptCenter.x << " " << ptCenter.y - 1.5f << " l\n"
               << ptCenter.x - 3 << " " << ptCenter.y + 1.5f << " l f\n"
               << "Q\n";
  }

  return CFX_ByteString(sAppStream);
}

void CPWL_Utils::DrawFillRect(CFX_RenderDevice* pDevice,
                              CFX_Matrix* pUser2Device,
                              const CFX_FloatRect& rect,
                              const FX_COLORREF& color) {
  CFX_PathData path;
  CFX_FloatRect rcTemp(rect);
  path.AppendRect(rcTemp.left, rcTemp.bottom, rcTemp.right, rcTemp.top);
  pDevice->DrawPath(&path, pUser2Device, nullptr, color, 0, FXFILL_WINDING);
}

void CPWL_Utils::DrawFillArea(CFX_RenderDevice* pDevice,
                              CFX_Matrix* pUser2Device,
                              const CFX_PointF* pPts,
                              int32_t nCount,
                              const FX_COLORREF& color) {
  CFX_PathData path;
  path.AppendPoint(pPts[0], FXPT_TYPE::MoveTo, false);
  for (int32_t i = 1; i < nCount; i++)
    path.AppendPoint(pPts[i], FXPT_TYPE::LineTo, false);

  pDevice->DrawPath(&path, pUser2Device, nullptr, color, 0, FXFILL_ALTERNATE);
}

void CPWL_Utils::DrawStrokeRect(CFX_RenderDevice* pDevice,
                                CFX_Matrix* pUser2Device,
                                const CFX_FloatRect& rect,
                                const FX_COLORREF& color,
                                float fWidth) {
  CFX_PathData path;
  CFX_FloatRect rcTemp(rect);
  path.AppendRect(rcTemp.left, rcTemp.bottom, rcTemp.right, rcTemp.top);

  CFX_GraphStateData gsd;
  gsd.m_LineWidth = fWidth;

  pDevice->DrawPath(&path, pUser2Device, &gsd, 0, color, FXFILL_ALTERNATE);
}

void CPWL_Utils::DrawStrokeLine(CFX_RenderDevice* pDevice,
                                CFX_Matrix* pUser2Device,
                                const CFX_PointF& ptMoveTo,
                                const CFX_PointF& ptLineTo,
                                const FX_COLORREF& color,
                                float fWidth) {
  CFX_PathData path;
  path.AppendPoint(ptMoveTo, FXPT_TYPE::MoveTo, false);
  path.AppendPoint(ptLineTo, FXPT_TYPE::LineTo, false);

  CFX_GraphStateData gsd;
  gsd.m_LineWidth = fWidth;

  pDevice->DrawPath(&path, pUser2Device, &gsd, 0, color, FXFILL_ALTERNATE);
}

void CPWL_Utils::DrawFillRect(CFX_RenderDevice* pDevice,
                              CFX_Matrix* pUser2Device,
                              const CFX_FloatRect& rect,
                              const CFX_Color& color,
                              int32_t nTransparency) {
  CPWL_Utils::DrawFillRect(pDevice, pUser2Device, rect,
                           color.ToFXColor(nTransparency));
}

void CPWL_Utils::DrawShadow(CFX_RenderDevice* pDevice,
                            CFX_Matrix* pUser2Device,
                            bool bVertical,
                            bool bHorizontal,
                            CFX_FloatRect rect,
                            int32_t nTransparency,
                            int32_t nStartGray,
                            int32_t nEndGray) {
  float fStepGray = 1.0f;

  if (bVertical) {
    fStepGray = (nEndGray - nStartGray) / rect.Height();

    for (float fy = rect.bottom + 0.5f; fy <= rect.top - 0.5f; fy += 1.0f) {
      int32_t nGray = nStartGray + (int32_t)(fStepGray * (fy - rect.bottom));
      CPWL_Utils::DrawStrokeLine(
          pDevice, pUser2Device, CFX_PointF(rect.left, fy),
          CFX_PointF(rect.right, fy),
          ArgbEncode(nTransparency, nGray, nGray, nGray), 1.5f);
    }
  }

  if (bHorizontal) {
    fStepGray = (nEndGray - nStartGray) / rect.Width();

    for (float fx = rect.left + 0.5f; fx <= rect.right - 0.5f; fx += 1.0f) {
      int32_t nGray = nStartGray + (int32_t)(fStepGray * (fx - rect.left));
      CPWL_Utils::DrawStrokeLine(
          pDevice, pUser2Device, CFX_PointF(fx, rect.bottom),
          CFX_PointF(fx, rect.top),
          ArgbEncode(nTransparency, nGray, nGray, nGray), 1.5f);
    }
  }
}

void CPWL_Utils::DrawBorder(CFX_RenderDevice* pDevice,
                            CFX_Matrix* pUser2Device,
                            const CFX_FloatRect& rect,
                            float fWidth,
                            const CFX_Color& color,
                            const CFX_Color& crLeftTop,
                            const CFX_Color& crRightBottom,
                            BorderStyle nStyle,
                            int32_t nTransparency) {
  float fLeft = rect.left;
  float fRight = rect.right;
  float fTop = rect.top;
  float fBottom = rect.bottom;

  if (fWidth > 0.0f) {
    float fHalfWidth = fWidth / 2.0f;

    switch (nStyle) {
      default:
      case BorderStyle::SOLID: {
        CFX_PathData path;
        path.AppendRect(fLeft, fBottom, fRight, fTop);
        path.AppendRect(fLeft + fWidth, fBottom + fWidth, fRight - fWidth,
                        fTop - fWidth);
        pDevice->DrawPath(&path, pUser2Device, nullptr,
                          color.ToFXColor(nTransparency), 0, FXFILL_ALTERNATE);
        break;
      }
      case BorderStyle::DASH: {
        CFX_PathData path;
        path.AppendPoint(
            CFX_PointF(fLeft + fWidth / 2.0f, fBottom + fWidth / 2.0f),
            FXPT_TYPE::MoveTo, false);
        path.AppendPoint(
            CFX_PointF(fLeft + fWidth / 2.0f, fTop - fWidth / 2.0f),
            FXPT_TYPE::LineTo, false);
        path.AppendPoint(
            CFX_PointF(fRight - fWidth / 2.0f, fTop - fWidth / 2.0f),
            FXPT_TYPE::LineTo, false);
        path.AppendPoint(
            CFX_PointF(fRight - fWidth / 2.0f, fBottom + fWidth / 2.0f),
            FXPT_TYPE::LineTo, false);
        path.AppendPoint(
            CFX_PointF(fLeft + fWidth / 2.0f, fBottom + fWidth / 2.0f),
            FXPT_TYPE::LineTo, false);

        CFX_GraphStateData gsd;
        gsd.SetDashCount(2);
        gsd.m_DashArray[0] = 3.0f;
        gsd.m_DashArray[1] = 3.0f;
        gsd.m_DashPhase = 0;

        gsd.m_LineWidth = fWidth;
        pDevice->DrawPath(&path, pUser2Device, &gsd, 0,
                          color.ToFXColor(nTransparency), FXFILL_WINDING);
        break;
      }
      case BorderStyle::BEVELED:
      case BorderStyle::INSET: {
        CFX_GraphStateData gsd;
        gsd.m_LineWidth = fHalfWidth;

        CFX_PathData pathLT;

        pathLT.AppendPoint(CFX_PointF(fLeft + fHalfWidth, fBottom + fHalfWidth),
                           FXPT_TYPE::MoveTo, false);
        pathLT.AppendPoint(CFX_PointF(fLeft + fHalfWidth, fTop - fHalfWidth),
                           FXPT_TYPE::LineTo, false);
        pathLT.AppendPoint(CFX_PointF(fRight - fHalfWidth, fTop - fHalfWidth),
                           FXPT_TYPE::LineTo, false);
        pathLT.AppendPoint(
            CFX_PointF(fRight - fHalfWidth * 2, fTop - fHalfWidth * 2),
            FXPT_TYPE::LineTo, false);
        pathLT.AppendPoint(
            CFX_PointF(fLeft + fHalfWidth * 2, fTop - fHalfWidth * 2),
            FXPT_TYPE::LineTo, false);
        pathLT.AppendPoint(
            CFX_PointF(fLeft + fHalfWidth * 2, fBottom + fHalfWidth * 2),
            FXPT_TYPE::LineTo, false);
        pathLT.AppendPoint(CFX_PointF(fLeft + fHalfWidth, fBottom + fHalfWidth),
                           FXPT_TYPE::LineTo, false);

        pDevice->DrawPath(&pathLT, pUser2Device, &gsd,
                          crLeftTop.ToFXColor(nTransparency), 0,
                          FXFILL_ALTERNATE);

        CFX_PathData pathRB;
        pathRB.AppendPoint(CFX_PointF(fRight - fHalfWidth, fTop - fHalfWidth),
                           FXPT_TYPE::MoveTo, false);
        pathRB.AppendPoint(
            CFX_PointF(fRight - fHalfWidth, fBottom + fHalfWidth),
            FXPT_TYPE::LineTo, false);
        pathRB.AppendPoint(CFX_PointF(fLeft + fHalfWidth, fBottom + fHalfWidth),
                           FXPT_TYPE::LineTo, false);
        pathRB.AppendPoint(
            CFX_PointF(fLeft + fHalfWidth * 2, fBottom + fHalfWidth * 2),
            FXPT_TYPE::LineTo, false);
        pathRB.AppendPoint(
            CFX_PointF(fRight - fHalfWidth * 2, fBottom + fHalfWidth * 2),
            FXPT_TYPE::LineTo, false);
        pathRB.AppendPoint(
            CFX_PointF(fRight - fHalfWidth * 2, fTop - fHalfWidth * 2),
            FXPT_TYPE::LineTo, false);
        pathRB.AppendPoint(CFX_PointF(fRight - fHalfWidth, fTop - fHalfWidth),
                           FXPT_TYPE::LineTo, false);

        pDevice->DrawPath(&pathRB, pUser2Device, &gsd,
                          crRightBottom.ToFXColor(nTransparency), 0,
                          FXFILL_ALTERNATE);

        CFX_PathData path;

        path.AppendRect(fLeft, fBottom, fRight, fTop);
        path.AppendRect(fLeft + fHalfWidth, fBottom + fHalfWidth,
                        fRight - fHalfWidth, fTop - fHalfWidth);

        pDevice->DrawPath(&path, pUser2Device, &gsd,
                          color.ToFXColor(nTransparency), 0, FXFILL_ALTERNATE);
        break;
      }
      case BorderStyle::UNDERLINE: {
        CFX_PathData path;
        path.AppendPoint(CFX_PointF(fLeft, fBottom + fWidth / 2),
                         FXPT_TYPE::MoveTo, false);
        path.AppendPoint(CFX_PointF(fRight, fBottom + fWidth / 2),
                         FXPT_TYPE::LineTo, false);

        CFX_GraphStateData gsd;
        gsd.m_LineWidth = fWidth;

        pDevice->DrawPath(&path, pUser2Device, &gsd, 0,
                          color.ToFXColor(nTransparency), FXFILL_ALTERNATE);
        break;
      }
    }
  }
}
