// $Id: MyValidator.h 175 2009-02-22 04:11:02Z felfert $
//
// Copyright (C) 2006 The OpenNX Team
// Author: Fritz Elfert
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#ifndef _MYVALIDATOR_H_
#define _MYVALIDATOR_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "MyValidator.h"
#endif

#include "wx/validate.h"

class KeyTypeCallback
{
public:
    virtual ~KeyTypeCallback() {};
    virtual void KeyTyped() = 0;
};

class MyValidator : public wxValidator
{
    DECLARE_DYNAMIC_CLASS(MyValidator)

public:

    typedef enum {
        MYVAL_ANY,
        MYVAL_NUMERIC,
        MYVAL_HEX,
        MYVAL_HOST,
        MYVAL_FILENAME,
    } MyValidatorType;


    MyValidator() { };
    MyValidator(bool*);
    MyValidator(int*);
    MyValidator(wxString*);
    MyValidator(wxArrayInt*);
    MyValidator(const MyValidator&);
    
    MyValidator(MyValidatorType, bool*);
    MyValidator(MyValidatorType, int*);
    MyValidator(MyValidatorType, wxString*);
    MyValidator(MyValidatorType, wxArrayInt*);
    MyValidator(MyValidatorType, const MyValidator&);
    
    virtual wxObject *Clone() const { return new MyValidator(*this); }
    bool Copy(const MyValidator& val);

    // Called when the value in the window must be validated.
    // This function can pop up an error message.
    virtual bool Validate(wxWindow * WXUNUSED(parent)) { return TRUE; }
    
    // Called to transfer data to the window
    virtual bool TransferToWindow();
    
    // Called to transfer data to the window
    virtual bool TransferFromWindow();
    
    void SetKeyTyped(KeyTypeCallback *);

    virtual ~MyValidator();

    void OnChar(wxKeyEvent& event);

    DECLARE_EVENT_TABLE()

protected:
  void Init();

  bool*       m_pBool;
  int*        m_pInt;
  wxString*   m_pString;
  wxArrayInt* m_pArrayInt;
  KeyTypeCallback *m_ktc;

private:

    MyValidatorType m_type;
};

#endif
// _MYVALIDATOR_H_
