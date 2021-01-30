/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef RADICALBUTTON_H
#define RADICALBUTTON_H

#include <QPushButton>
#include <QString>

class QMouseEvent;

class RadicalButton : public QPushButton
{
  Q_OBJECT

  public:
             RadicalButton( const QString &text, QWidget *parent );
    virtual ~RadicalButton();
    /**
     * Overriding QPushButton's event for mousewheel events on a disabled button
     */
    bool     event( QEvent *event ) override;
    QSize    minimumSizeHint() const override;
    QSize    sizeHint() const override;

    typedef enum
    {
      Normal,         /**< Normal button */
      Selected,       /**< This button has been selected (checked) */
      NotAppropriate, /**< Due to other selected buttons: disabled */
      Related,        /**< Display only this radical and related ones: italics? */
      Hidden          /**< Not related (to above), so hide() */
    } ButtonStatus;

  signals:
    void userClicked( const QString &text, RadicalButton::ButtonStatus status );

  public slots:
    void mousePressEvent( QMouseEvent *e ) override;
    void mouseReleaseEvent( QMouseEvent *e ) override;
    void resetButton();
    void setStatus( RadicalButton::ButtonStatus newStatus );

  private:
    ButtonStatus m_status;
};

#endif
