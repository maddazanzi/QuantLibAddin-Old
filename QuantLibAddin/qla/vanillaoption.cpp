
/*
 Copyright (C) 2004, 2005 Eric Ehlers

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#if defined(HAVE_CONFIG_H)     // Dynamically created by configure
    #include <qla/config.hpp>
#endif
#include <qla/vanillaoption.hpp>
#include <qla/optionutils.hpp>

namespace QuantLibAddin {

    VanillaOption::VanillaOption(ObjHandler::ArgStack &args) {
        long timeSteps                  = ObjHandler::Args<long>::popArg(args);
        std::string engineID            = ObjHandler::Args<std::string>::popArg(args);
        long settlementDate             = ObjHandler::Args<long>::popArg(args);
        long exerciseDate               = ObjHandler::Args<long>::popArg(args);
        std::string exerciseID          = ObjHandler::Args<std::string>::popArg(args);
        double strike                   = ObjHandler::Args<double>::popArg(args);
        std::string payoffID            = ObjHandler::Args<std::string>::popArg(args);
        std::string optionTypeID        = ObjHandler::Args<std::string>::popArg(args);
        std::string handleStochastic    = ObjHandler::Args<std::string>::popArg(args);

        boost::shared_ptr<StochasticProcess> stochasticProcess =
            boost::dynamic_pointer_cast<StochasticProcess>
            (QL_OBJECT_GET(handleStochastic));
        if (!stochasticProcess)
            QL_FAIL("VanillaOption: error retrieving object " + handleStochastic);

        boost::shared_ptr<QuantLib::StrikedTypePayoff> payoff =
            IDtoPayoff(optionTypeID, payoffID, strike);
        boost::shared_ptr<QuantLib::Exercise> exercise = 
            IDtoExercise(exerciseID, exerciseDate, settlementDate);
        boost::shared_ptr<QuantLib::PricingEngine> pricingEngine =
            IDtoEngine(engineID, timeSteps);
        const boost::shared_ptr<QuantLib::BlackScholesProcess> stochasticProcessQL = 
            boost::static_pointer_cast<QuantLib::BlackScholesProcess>
            (stochasticProcess->getReference());
        vanillaOption_ = boost::shared_ptr<QuantLib::VanillaOption>(
            new QuantLib::VanillaOption(
                stochasticProcessQL, 
                payoff, 
                exercise, 
                pricingEngine));
        ObjHandler::any_ptr any_npv(new boost::any(vanillaOption_->NPV()));
        ObjHandler::any_ptr any_engine(new boost::any(std::string(engineID)));
        ObjHandler::ObjectProperty prop_npv(FIELD_NPV, any_npv);
        ObjHandler::ObjectProperty prop_engine(FIELD_ENGINE, any_engine);
        properties_.push_back(prop_npv);
        properties_.push_back(prop_engine);
    }

    void VanillaOption::setEngine(
            const std::string &engineID,
            const long &timeSteps) {
        boost::shared_ptr<QuantLib::PricingEngine> pricingEngine =
            IDtoEngine(engineID, timeSteps);
        vanillaOption_->setPricingEngine(pricingEngine);
        *properties_[IDX_NPV]() = vanillaOption_->NPV();
        *properties_[IDX_ENGINE]() = engineID;
    }

}

