#include "thlocale.h"
#include "thparse.h"
#include "thbuffer.h"
#include "thexception.h"
#include "thlang.h"
#include "thinit.h"

#define LCBUFFLEN 1024
char lcbuff [LCBUFFLEN];
char lcfmt1 [] = {37, 49, 46, 49, 102, 0}; // "%1.1f";
char lcfmt2 [] = {37, 49, 48, 46, 49, 102, 0}; // "%10.1f";

thlocale::thlocale()
{
	this->units = TTLC_UNITS_METRIC;
	this->lang = thini.lang;
}


static const thstok thtt_locale_units[] = {
  {"imperial", TTLC_UNITS_IMPERIAL},
  {"metric", TTLC_UNITS_METRIC},
  {NULL, TTLC_UNKNOWN}
};

	
void thlocale::parse_units(char * cc)
{
	this->units = thmatch_token(cc, thtt_locale_units);
	if (this->units == TTLC_UNKNOWN)
		ththrow(("unknown units -- %s", cc))
}
	

char * thlocale::format_length(double length, int prec, int total)
{
	switch (this->units) {
		case TTLC_UNITS_IMPERIAL:
			length /= 0.3048;
		  break;
	}
	if (total < 10) {
			lcfmt1[1] = total + 48;
			lcfmt1[3] = prec + 48;
      snprintf(lcbuff, LCBUFFLEN, lcfmt1, length);
	} else {
			lcfmt2[1] = total % 10 + 48;
			lcfmt2[2] = total / 10 + 48;
			lcfmt2[4] = prec + 48;
      snprintf(lcbuff, LCBUFFLEN, lcfmt2, length);
	}
	lcbuff[LCBUFFLEN - 1] = 0;
	return lcbuff;
}


char * thlocale::format_length_units()
{
	switch (this->units) {
		case TTLC_UNITS_IMPERIAL:
			return "ft";
	  default:
			return "m";
	}
}

char * thlocale::format_i18n_length_units()
{
	switch (this->units) {
		case TTLC_UNITS_IMPERIAL:
			return thT("units ft", this->lang);
	  default:
			return thT("units m", this->lang);
	}
}

double thlocale::convert_length(double length)
{
	switch (this->units) {
		case TTLC_UNITS_IMPERIAL:
			length /= 0.3048;
		  break;
	}
	return length;
}

thlocale thdeflocale;

