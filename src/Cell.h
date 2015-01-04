//
//  Copyright (C) 2004-2014 Andrej Vodopivec <andrej.vodopivec@gmail.com>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

/*!\file
  
  The definition of the base class of all cells.
 */

#ifndef CELL_H
#define CELL_H

#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define MIN(a,b) ((a)>(b) ? (b) : (a))
#define ABS(a) ((a)>=0 ? (a) : -(a))
#define SCALE_PX(px, scale) ((int)((double)((px)*(scale)) + 0.5))

#define MC_CELL_SKIP 0
#define MC_BASE_INDENT 12
#define MC_LINE_SKIP 2
#define MC_TEXT_PADDING 1

#define MC_GROUP_SKIP 20
#define MC_GROUP_LEFT_INDENT 15

#if defined __WXMAC__
 #define MC_EXP_INDENT 2
 #define MC_MIN_SIZE 10
 #define MC_MAX_SIZE 36
#else
 #define MC_EXP_INDENT 4
 #define MC_MIN_SIZE 8
 #define MC_MAX_SIZE 36
#endif

#include <wx/wx.h>
#include "CellParser.h"
#include "TextStyle.h"

/*! The supported types of math cells
 */
enum {
  MC_TYPE_DEFAULT,
  MC_TYPE_MAIN_PROMPT,
  MC_TYPE_PROMPT,
  MC_TYPE_LABEL,       //!< An output label generated by maxima
  MC_TYPE_INPUT,       //!< An input cell
  MC_TYPE_ERROR,       //!< An error output by maxima
  MC_TYPE_TEXT,        //!< Text that isn't passed to maxima
  MC_TYPE_SUBSECTION,  //!< A subsection name
  MC_TYPE_SECTION,     //!< A section name
  MC_TYPE_TITLE,       //!< The title of the document
  MC_TYPE_IMAGE,       //!< An image
  MC_TYPE_SLIDE,       //!< An animation created by the with_slider_* maxima commands
  MC_TYPE_GROUP        //!< A group cells that bundles several individual cells together
};

/*!
  The base class all cell types are derived from

  Besides the cell types that are directly user visible there are cells for several
  kinds of items that are displayed in a special way like abs() statements (displayed
  as horizontal rules), subscripts, superscripts and exponents.
  Another important concept realized realized by a clas derived from this one is 
  the group cell that groups all things that are foldable in the gui like:
   - A combination of maxima input with the output, the input prompt and the output 
     label.
   - A chapter or a section and
   - Images with their title (or the input cells that generated them)
   .

  \attention Derived classes must test if m_next equals NULL and if it doesn't
  they have to delete() it.
 */
class Cell
{
public:
  Cell();
  virtual ~Cell();
  /*! Copy this object

    \return A copy of this object with (if the parameter all was true) all 
    following cells attached.
   */
  virtual Cell* Copy() = 0;
  
  //! Do we want this cell to start with a linebreak?
  void BreakLine(bool breakLine) { m_breakLine = breakLine; }
  //! Do we want this cell to start with a pagebreak?
  void BreakPage(bool breakPage) { m_breakPage = breakPage; }
  //! Are we allowed to break a line here?
  bool BreakLineHere();
  //! Does this cell begin begin with a manual linebreak?
  bool ForceBreakLineHere() { return m_forceBreakLine; }
  //! Does this cell begin begin with a manual page break?  
  bool BreakPageHere() { return m_breakPage; }
  virtual bool BreakUp() { return false; }
  /*! Is a part of this object inside a certain rectangle?

    \param big The rectangle to test for collision with this object
   */
  bool ContainsRect(wxRect& big);
  /*! Is a given point inside this object?

    \param point The point to test for collision with this object
   */
  bool ContainsPoint(wxPoint& point)
  {
    return GetRect().Contains(point);
  }
  void CopyData(Cell *s, Cell *t);

  /*! Draw this object

    \param point The x and y position this object is drawn at
    \param fontsize The font size that is to be used
   */
  virtual void Draw(CellParser& parser, wxPoint point, int fontsize)=0;
  //! Draws a box around this object.
  virtual void DrawBoundingBox(wxDC& dc, int border = 0);
  bool DrawThisCell(CellParser& parser, wxPoint point);

  /*! Insert (or remove) a forced linebreak at the beginning of this cell.

    \param force
     - true: Insert a forced linebreak
     - false: Remove the forced linebreak
   */
  void ForceBreakLine(bool force) { m_forceBreakLine = m_breakLine = force; }

  //! Invalidate the cached size information.
  virtual void InvalidateSizeInformation();

  
  //! Get the total height of this cell
  int GetHeight() { return m_height; }
  //! Get the width of this cell
  int GetWidth() { return m_width; }
  /*! Get the distance between the top and the center of this cell.

    Remember that (for example with double fractions) the center does not have to be in the 
    middle of a cell even if this object is --- by definition --- center-aligned.
   */
  int GetCenter() { return m_center; }
  /*! Get the distance between the center and the bottom of this cell


    Remember that (for example with double fractions) the center does not have to be in the 
    middle of a cell even if this object is --- by definition --- center-aligned.
   */
  int GetDrop() { return m_height - m_center; }

  /*! 
    Returns the type of this cell.
   */
  int GetType() { return m_type; }
  /*! Get the maximum difference between the center and the top of this line

    \todo This piece of code is still recursive.
    \todo Move to CellList?
  */
  int GetMaxCenter();
  int GetMaxDrop();
  /*! Get the maximum distance between center and 

    \todo Still containing recursive code that is risking exceeding the platform's
    current stack limit.
    \todo Move to CellList?
   */
  int GetMaxHeight();
  //! Get the x position of the top left of this cell
  int GetCurrentX() { return m_currentPoint.x; }
  //! Get the y position of the top left of this cell
  int GetCurrentY() { return m_currentPoint.y; }
  /*! Get the smallest rectangle this object fits in
   */
  virtual wxRect GetRect();
  virtual wxString GetDiffPart();
  //! Recalculate the height of the object and the difference between top and center
  virtual void RecalculateSize(CellParser& parser, int fontsize)=0;
  //! Marks the width as to be recalculated on query.
  virtual void RecalculateWidths(CellParser& parser, int fontsize);
  void ResetData();
  void ResetSize() { m_width = m_height = -1; }

  void SetSkip(bool skip) { m_bigSkip = skip; }
  void SetType(int type);
  int GetStyle(){ return m_textStyle; }	//l'ho aggiunto io

  void SetPen(CellParser& parser);
  void SetHighlight(bool highlight) { m_highlight = highlight; }
  virtual void SetExponentFlag() { }
  virtual void SetValue(wxString text) { }
  virtual wxString GetValue() { return wxEmptyString; }

  void SelectRect(wxRect& rect, Cell** first, Cell** last);
  void SelectFirst(wxRect& rect, Cell** first);
  void SelectLast(wxRect& rect, Cell** last);
  /*! Select a rectangle that is created by a cell inside this cell.

    \attention This method has to be overridden by children of the 
    Cell class.
  */
  virtual void SelectInner(wxRect& rect, Cell** first, Cell** last);

  virtual bool IsOperator();
  virtual bool IsShortNum() { return false; }

  Cell* GetParent();

  //! Returns the object's representation as a string.
  virtual wxString ToString()=0;
  //! Convert this cell to its LaTeX representation
  virtual wxString ToTeX()=0;
  //! Convert this cell to an representation fit for saving in a .wxmx file
  virtual wxString ToXML()=0;

  void UnsetPen(CellParser& parser);
  virtual void Unbreak();

  /*! The group cell this list of cells belongs to.
    
    Reads NULL, if no parent cell has been set.    
   */
  Cell *m_group;
  Cell *m_nextToDraw, *m_previousToDraw;
  wxPoint m_currentPoint;  // Current point in console (the center of the cell)
  bool m_bigSkip;
  //! true means: Add a linebreak to the end of this cell.
  bool m_isBroken;
  bool m_isHidden;
  /*! Determine if this cell contains text that won't be passed to maxima

    \return true, if this is a text cell, a title cell, a section or a subsection cell.
   */
  bool IsComment()
  {
    return m_type == MC_TYPE_TEXT || m_type == MC_TYPE_SECTION ||
           m_type == MC_TYPE_SUBSECTION || m_type == MC_TYPE_TITLE;
  }

  virtual void ProcessEvent(wxKeyEvent& event)=0;
  virtual bool ActivateCell() { return false; }
  virtual bool AddEnding() { return false; }
  virtual void SelectPointText(wxDC &dc, wxPoint& point)=0;
  virtual void SelectRectText(wxDC &dc, wxPoint& one, wxPoint& two)=0;
  virtual void PasteFromClipboard(bool primary = false)=0;
  virtual bool CopyToClipboard() { return false; }
  virtual bool CutToClipboard() { return false; }
  virtual void SelectAll() = 0;
  virtual bool CanCopy() { return false; }
  virtual void SetMatchParens(bool match)=0;
  virtual wxPoint PositionToPoint(CellParser& parser, int pos = -1) { return wxPoint(-1, -1); }
  virtual bool IsDirty() { return false; }
  virtual void SwitchCaretDisplay()=0;
  virtual void SetFocus(bool focus)=0;
  void SetForeground(CellParser& parser);
  virtual bool IsActive() { return false; }
  virtual void SetParent(Cell *parent);
  void SetStyle(int style) { m_textStyle = style; }
  bool IsMath();
  void SetAltCopyText(wxString text) { m_altCopyText = text; }
  /*! Caches the maximum difference between the center and the top of this line

    Can be queried by GetMaxCenter(); The value -1 means that this information 
    still needs to be calculated.
   */
  int m_maxCenter;
  /*! Caches the maximum difference between the center and the bottom of this line

    Can be queried by GetMaxDrop(); The value -1 means that this information 
    still needs to be calculated.
   */
  int m_maxDrop;
protected:
  /*! Attach a copy of the list of cells that follows this one to a cell
  */
  Cell* CopyRestFrom(Cell *src);
  
  //! The height of this cell
  int m_height;
  //! The width of this cell
  int m_width;
  int m_center;
  int m_type;
  int m_textStyle;
  //! Does this cell begin with a forced page break?
  bool m_breakPage;
  //! Are we allowed to add a line break before this cell?
  bool m_breakLine;
  //! true means we forcce this cell to begin with a line break.  
  bool m_forceBreakLine;
  bool m_highlight;
  wxString m_altCopyText; // m_altCopyText is not check in all cells!
};

#endif // CELL_H
