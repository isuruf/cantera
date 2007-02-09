/**
 *  @file KineticsFactory.cpp
 */

/*
 * $Author$
 * $Revision$
 * $Date$
 */

// Copyright 2001  California Institute of Technology


#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "KineticsFactory.h"

#include "GasKinetics.h"
#include "GRI_30_Kinetics.h"
#include "InterfaceKinetics.h"
#include "EdgeKinetics.h"
#include "importCTML.h"

using namespace std;

namespace Cantera {

    KineticsFactory* KineticsFactory::s_factory = 0;

    static int ntypes = 5;
    static string _types[] = {"none", "GasKinetics", "GRI30", "Interface", "Edge"};
    static int _itypes[]   = {0, cGasKinetics, cGRI30, cInterfaceKinetics, cEdgeKinetics};

    /**
     * Return a new kinetics manager that implements a reaction
     * mechanism specified in a CTML file. In other words, the
     * kinetics manager, given the rate constants and formulation of the
     * reactions that make up a kinetics mechanism, is responsible for
     * calculating the rates of progress of the reactions and for
     * calculating the source terms for species.
     * 
     * Input
     * ------
     *  phaseData = This is an XML_Node that contains the xml data
     *              describing the phase. Of particular note to this
     *              routine is the child xml element called "kinetics".
     *              The element has one attribute called "model",
     *              with a string value. The value of this string
     *              is used to decide which kinetics manager is used
     *              to calculate the reacton mechanism. 
     *
     * Return
     * ---------
     *  Pointer to the new kinetics manager. 
     */

    Kinetics* KineticsFactory::
    newKinetics(XML_Node& phaseData, vector<ThermoPhase*> th) {
	/*
	 * Look for a child of the xml element phase called
	 * "kinetics". It has an attribute name "model".
	 * Store the value of that attribute in the variable kintype
	 */
        string kintype = phaseData.child("kinetics")["model"];
	/*
	 * look up the string kintype in the list of known
	 * kinetics managers (list is kept at the top of this file).
	 * Translate it to an integer value, ikin. 
	 */
        int ikin=-1;
        int n;
        for (n = 0; n < ntypes; n++) {
	  if (kintype == _types[n]) ikin = _itypes[n];
        }
	/*
	 * Assign the kinetics manager based on the value of ikin.
	 * Kinetics managers are classes derived from the base 
	 * Kinetics class. Unknown kinetics managers will throw a
	 * CanteraError here.
	 */
        Kinetics* k=0;
        switch (ikin) {
            
        case 0:
            k = new Kinetics;
            break;

        case cGasKinetics:
            k = new GasKinetics;
            break;

        case cGRI30:
            k = new GRI_30_Kinetics;
            break;

        case cInterfaceKinetics:
            k = new InterfaceKinetics;
            break;

        case cEdgeKinetics:
            k = new EdgeKinetics;
            break;

        default:
	    throw UnknownKineticsModel("KineticsFactory::newKinetics", 
				       kintype);
        }

        // Now that we have the kinetics manager, we can
        // import the reaction mechanism into it.
        importKinetics(phaseData, th, k);

        // Return the pointer to the kinetics manager
        return k;
    }


    /**
     * Return a new, empty kinetics manager.
     */
    Kinetics* KineticsFactory::newKinetics(string model) {

        int ikin = -1;
        int n;
        for (n = 0; n < ntypes; n++) {
            if (model == _types[n]) ikin = _itypes[n];
        }
        Kinetics* k=0;
        switch (ikin) {

        case cGasKinetics:
            k = new GasKinetics;
            break;

        case cGRI30:
            k = new GRI_30_Kinetics;
            break;

        case cInterfaceKinetics:
            k = new InterfaceKinetics;
            break;

        default:
	    throw UnknownKineticsModel("KineticsFactory::newKinetics", 
				       model);
        }
        return k;
    }

}
