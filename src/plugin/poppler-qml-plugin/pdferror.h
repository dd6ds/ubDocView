/*
 * Copyright (C) 2016
 *          Stefano Verzegnassi <verzegnassi.stefano@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PDFERROR_H
#define PDFERROR_H

#include <QObject>

class PopplerError : public QObject
{
    Q_OBJECT
    Q_ENUMS(Error)

public:
    enum Error {
        NoError                   = 0,
        FileNotFound              = 1,
        DocumentLocked            = 2
    };
};

#endif // PDFERROR_H
