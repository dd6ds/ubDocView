/*
 * Copyright (C) 2015 Roman Shchekin
 * Copyright (C) 2015-2016 Stefano Verzegnassi <stefano92.100@gmail.com>
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
 */

#include "pdfrendertask.h"

bool PdfRenderTask::canBeRunInParallel(AbstractRenderTask* prevTask)
{
    Q_ASSERT(prevTask != nullptr);
    return true;
}

QImage PdfRenderTask::doWork()
{
    return m_document.data()->paintPage(m_page, m_zoom, m_area, PdfDocument::Rotation(m_rotation));
}
