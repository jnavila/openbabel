/**********************************************************************
obmolecformat.h - Subclass of OBFormat for conversion of OBMol.

Copyright (C) 2005 Chris Morley

This file is part of the Open Babel project.
For more information, see <http://openbabel.sourceforge.net/>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
***********************************************************************/

#ifndef OB_MOLECULEFORMAT_H
#define OB_MOLECULEFORMAT_H

#include "mol.h"
#include "obconversion.h"

namespace OpenBabel {

//! \brief An OBFormat convenience subclass for conversion to/from OBMol data
//!
//! An OBFormat which converts to and/or from OBMol can derive from this class
//! to save duplicating the ReadChemObject() and/or WriteChemObject() methods.
//! Derive directly from OBFormat if the object converted is not OBMol or 
//! if interaction with the framework is required during the execution 
//! of ReadMolecule() or WriteMolecule(), as for example in CMLFormat
class OBMoleculeFormat : public OBFormat
{
public:
	/// The "Convert" interface functions
	virtual bool ReadChemObject(OBConversion* pConv)
	{
	  std::istream &ifs = *pConv->GetInStream();
	  if (ifs.peek() == EOF || !ifs.good())
	    return false;

		static OBMol* pmol;

		    std::string auditMsg = "OpenBabel::Read molecule ";
		    std::string description(Description());
		    auditMsg += description.substr(0,description.find('\n'));
		    obErrorLog.ThrowError(__FUNCTION__,
					  auditMsg,
					  obAuditMsg);

		//With j option, reuse pmol except for the first mol
		if(!pConv->IsOption("j",OBConversion::GENOPTIONS) || pConv->IsFirstInput())
			pmol = new OBMol;
		
		bool ret=ReadMolecule(pmol,pConv);
		if(ret && pmol->NumAtoms() > 0) //Do transformation and return molecule
			pConv->AddChemObject(pmol->DoTransformations(pConv->GetOptions(OBConversion::GENOPTIONS)));
		else
			pConv->AddChemObject(NULL);

		return ret;
	};
	
	virtual bool WriteChemObject(OBConversion* pConv)
	{
		//Retrieve the target OBMol
		OBBase* pOb = pConv->GetChemObject();
		OBMol* pmol = dynamic_cast<OBMol*> (pOb);
		bool ret=false;
		if(pmol && pmol->NumAtoms() > 0)
		{	
			ret=true;

			std::string auditMsg = "OpenBabel::Write molecule ";
			std::string description(Description());
			auditMsg += description.substr(0,description.find('\n'));
			obErrorLog.ThrowError(__FUNCTION__,
					      auditMsg,
					      obAuditMsg);

			if(!pConv->IsOption("j",OBConversion::GENOPTIONS) || pConv->IsLast()) //With j option, output only at end
			{
				ret=WriteMolecule(pmol,pConv);
				delete pOb;
			}
		}
		return ret;
	};
};

}
#endif //OB_MOLECULEFORMAT_H

//! \file obmolecformat.h
//! \brief Subclass of OBFormat for conversion of OBMol.
