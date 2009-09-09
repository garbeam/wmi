// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: prompt.cpp 734 2004-09-27 18:15:45Z garbeam $

#include "prompt.h"

#include "binder.h"

Prompt::Prompt(string prompt, QueryNamesForPrefix toQuery)
{
    prompt_ = prompt;
    toQuery_ = toQuery;
}

string Prompt::prompt() const {
    return prompt_;
}

QueryNamesForPrefix Prompt::toQuery() const {
    return toQuery_;
}
