/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "musicxmlvalidation.h"
#include <QFile>
#ifdef MUSICXML_NO_VALIDATION
using namespace mu;
using namespace mu::iex::musicxml;
using namespace mu::engraving;

Err MusicxmlValidation::validate(const String&, const ByteArray&)
{
    return Err::NoError;
}

#else

#include <libxml/xmlschemastypes.h>
#include <libxml/catalog.h>

#include <QMessageBox>


#include "engraving/dom/mscore.h"

#include "musicxmlsupport.h"

#include "log.h"

using namespace mu;
using namespace mu::iex::musicxml;
using namespace mu::engraving;


//---------------------------------------------------------
//   musicXMLValidationErrorDialog
//---------------------------------------------------------

/**
 Show a dialog displaying the MusicXML validation error(s)
 and asks the user if he wants to try to load the file anyway.
 Return QMessageBox::Yes (try anyway) or QMessageBox::No (don't)
 */
static QString detailedText;
static int musicXMLValidationErrorDialog(void *ctxt,const char *fmt,...)
  {
    const int MSG_BUF_SIZE =1024;
    std::cout << fmt <<__FILE__ << __LINE__;
    std::va_list argp;
    va_start(argp, fmt);
    char  detailedErrorBuf[MSG_BUF_SIZE];
    vsnprintf(detailedErrorBuf,MSG_BUF_SIZE,fmt,argp);
    detailedText+=QString(detailedErrorBuf);
    va_end(argp);
    return 0;
  }
Err MusicxmlValidation::validate(const String& name, const ByteArray& data)
{
    xmlLoadCatalog(MUSICXML_CATALOG_FILE);
    bool valid = false;
    xmlDocPtr doc;
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;
    ctxt = xmlSchemaNewParserCtxt(MUSICXML_SCHEMA);
    xmlLineNumbersDefault(1);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) musicXMLValidationErrorDialog, (xmlSchemaValidityWarningFunc) musicXMLValidationErrorDialog,ctxt);

    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);


    doc = xmlReadFile(name.toStdString().c_str(), NULL, 0);

       if (doc == NULL)
        {
        std::cout << "Could not parse  " <<  name.toStdString() << "\n";
        }
    else
        {
        xmlSchemaValidCtxtPtr ctxt;
        int ret;

        ctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) musicXMLValidationErrorDialog, (xmlSchemaValidityWarningFunc) musicXMLValidationErrorDialog,ctxt);
        ret = xmlSchemaValidateDoc(ctxt, doc);
        if (ret == 0)
            {
	valid = true;
	detailedText.clear();
            }
        else if (ret > 0)
         {
	   valid = false;
	 }else {
	  valid = false;
	}
        xmlSchemaFreeValidCtxt(ctxt);
        xmlFreeDoc(doc);
        }

       if(schema != NULL)
      xmlSchemaFree(schema);
    if (!valid) {
        LOGD("importMusicXml() file '%s' is not a valid MusicXML file", muPrintable(name));
        QString strErr = qtrc("iex_musicxml", "File “%1” is not a valid MusicXML file.").arg(name);
        if (MScore::noGui) {
            return Err::NoError;         // might as well try anyhow in converter mode
        }

    QMessageBox errorDialog;
    errorDialog.setIcon(QMessageBox::Question);
    errorDialog.setText(qtrc("iexmusic_xml","Xml validation error"));
    errorDialog.setInformativeText(qtrc("iex_musicxml", "Do you want to try to load this file anyway?"));
    errorDialog.setDetailedText(detailedText);
    errorDialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    errorDialog.setDefaultButton(QMessageBox::No);
    if (errorDialog.exec() != QMessageBox::Yes) {
      detailedText.clear();
      return Err::UserAbort;
    }
    detailedText.clear();
    return Err::NoError;

    }
    return Err::NoError;
}

#endif // MUSICXML_NO_VALIDATION
