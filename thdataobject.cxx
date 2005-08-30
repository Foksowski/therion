/**
 * @file thdataobject.cxx
 */
  
/* Copyright (C) 2000 Stacho Mudrak
 * 
 * $Date: $
 * $RCSfile: $
 * $Revision: $
 *
 * -------------------------------------------------------------------- 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * --------------------------------------------------------------------
 */
 
#include "thdataobject.h"
#include "thexception.h"
#include "thchenc.h"
#include "thsurvey.h"

thdataobject::thdataobject()
{
  this->db = NULL;
  
  this->name = "";
  this->title = "";
  this->selected = false;
  this->selected_number = 0;

  this->stnpref = NULL;
  this->stnsuff = NULL;
  
  this->fsptr = NULL;
  this->nsptr = NULL;
  this->psptr = NULL;
  
  this->revision = 0;
}


thdataobject::~thdataobject()
{
}

  
bool operator < (const thdataobject_author & a1, 
    const thdataobject_author & a2)
{
  if (a1.rev < a2.rev)
    return true;
  if ((a1.rev == a2.rev) && (a1.name < a2.name))
    return true;
  return false;
}


bool operator < (const thdataobject_copyright & c1, 
      const thdataobject_copyright & c2)
{
  if (c1.rev < c2.rev)
    return true;
  if ((c1.rev == c2.rev) && (strcmp(c1.name, c2.name) < 0))
    return true;
  return false;
}


void thdataobject::assigndb(thdatabase * pdb) 
{
  this->db = pdb;
}


int thdataobject::get_class_id() 
{
  return TT_DATAOBJECT_CMD;
}


bool thdataobject::is(int class_id)
{
  if (class_id == TT_DATAOBJECT_CMD)
    return true;
  else
    return false;
}

int thdataobject::get_cmd_nargs() 
{
  return 0;
}


char * thdataobject::get_cmd_end()
{
  return NULL;
}


bool thdataobject::get_cmd_ends_state() {
  return false;
}


bool thdataobject::get_cmd_ends_match(char * cmd) {
  return false;
}


char * thdataobject::get_cmd_name()
{
  return NULL;
}


thcmd_option_desc thdataobject::get_cmd_option_desc(char * opts)
{
  int id = thmatch_token(opts, thtt_dataobject_opt);
  switch (id) {
    case TT_DATAOBJECT_AUTHOR:
    case TT_DATAOBJECT_COPYRIGHT:
    case TT_DATAOBJECT_STNS:
      return thcmd_option_desc(id, 2);
    default:
      return thcmd_option_desc(id);
  }
}


void thdataobject::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{  
  switch (cod.id) {

    case TT_DATAOBJECT_NAME:
      if (th_is_extkeyword(*args))
        this->name = this->db->strstore(*args);
      else 
        ththrow(("invalid keyword -- %s", *args));
      break;
    
    case TT_DATAOBJECT_STNS:
      switch (this->get_class_id()) {
        case TT_DATA_CMD:
        case TT_SCRAP_CMD:
          break;
        default:
          ththrow(("station-names specification not valid for %s", this->get_cmd_name()))
          break;  
      }
      if (strlen(args[0]) == 0)
        this->stnpref = NULL;
      else {
        if (th_is_extkeyword(args[0]))
          this->stnpref = this->db->strstore(args[0]);
        else 
          ththrow(("invalid keyword -- %s", args[0]));
      }
      if (strlen(args[1]) == 0)
        this->stnsuff = NULL;
      else {
        if (th_is_extkeyword(args[1]))
          this->stnsuff = this->db->strstore(args[1]);
        else 
          ththrow(("invalid keyword -- %s", args[1]));
      }
      break;

    case TT_DATAOBJECT_TITLE:
      switch (this->get_class_id()) {
        case TT_DATA_CMD:
        case TT_SURVEY_CMD:
        case TT_MAP_CMD:
        case TT_SCRAP_CMD:
        case TT_GRADE_CMD:
        case TT_LAYOUT_CMD:
          if (cod.nargs > 1)
            ththrow(("multiple option arguments -- title"))
          thencode(&(this->db->buff_enc), *args, argenc);
          this->title = this->db->strstore(this->db->buff_enc.get_buffer());
          break;
        default:
          ththrow(("title specification not allowed for this object"));
          break;
      }
      break;
      
    case TT_DATAOBJECT_AUTHOR:
      switch (this->get_class_id()) {
        case TT_DATA_CMD:
        case TT_SCRAP_CMD:
          if (cod.nargs > 2)
            ththrow(("too many option arguments -- author"))
          this->dotmp_date.parse(args[0]);
          thencode(&(this->db->buff_enc), args[1], argenc);
          this->dotmp_person.parse(this->db, this->db->buff_enc.get_buffer());
          this->dotmp_author = thdataobject_author(this->dotmp_person,
              this->revision);
          this->author_map[this->dotmp_author].join(this->dotmp_date);
          break;
        default:
          ththrow(("author specification not allowed for this object"));
          break;
      }
      break;
      
    case TT_DATAOBJECT_COPYRIGHT:
      switch (this->get_class_id()) {
        case TT_DATA_CMD:
        case TT_SCRAP_CMD:
          if (cod.nargs > 2)
            ththrow(("too many option arguments -- copyright"))
          this->dotmp_date.parse(args[0]);
          thencode(&(this->db->buff_enc), args[1], argenc);
          this->dotmp_copyright = 
            thdataobject_copyright(
            this->db->strstore(this->db->buff_enc.get_buffer(), true), 
            this->revision);
          this->copyright_map[this->dotmp_copyright].join(this->dotmp_date);
          break;
        default:
          ththrow(("copyright specification not allowed for this object"));
          break;
      }
      break;
        
    default:
      ththrow(("unknown option -- %s", args[0]));

  }
}


int thdataobject::get_id()
{
  return this->id;
}


void thdataobject::self_delete()
{
  delete this;
}


int thdataobject::get_context()
{
  return THCTX_SURVEY;
}


char * thdataobject::get_name()
{
  return this->name;
}


char * thdataobject::get_title()
{
  return this->title;
}


void thdataobject::throw_source()
{
  if (strlen(thexc.get_buffer()) > 0)
    thexc.insspf(" -- ");
  thexc.insspf("%s [%d]", this->source.name, this->source.line);
}

void thdataobject::self_print(FILE * outf)
{
  if (strlen(this->name) > 0)
    fprintf(outf,"%s (%ld:0x%lx) -- %s\n", this->get_class_name(), this->id, (unsigned long) this, this->name);
  else
    fprintf(outf,"%s (%ld:0x%lx)\n", this->get_class_name(), this->id, (unsigned long) this);  

  this->self_print_properties(outf);

  if (strlen(this->name) > 0)
    fprintf(outf,"end %s -- %s\n", this->get_class_name(), this->name);
  else
    fprintf(outf,"end %s\n", this->get_class_name());  
}


void thdataobject::self_print_properties(FILE * outf)
{

  fprintf(outf,"thdataobject:\n");

  thdo_author_map_type::iterator aii;
  for (aii = this->author_map.begin();
        aii != this->author_map.end(); aii++) {
    if (aii->first.rev == 0)
      fprintf(outf,"\tauthor: %s \"%s/%s\"\n", aii->second.get_str(),
          aii->first.name.get_n1(), aii->first.name.get_n2());
    else
      fprintf(outf,"\tauthor: %s \"%s/%s\" (rev. %ld)\n", aii->second.get_str(),
          aii->first.name.get_n1(), aii->first.name.get_n2(), aii->first.rev);
  }
  
  thdo_copyright_map_type::iterator cii;
  for (cii = this->copyright_map.begin();
        cii != this->copyright_map.end(); cii++) {
    if (cii->first.rev == 0)
      fprintf(outf,"\tcopyright: %s \"%s\"\n", cii->second.get_str(),
          cii->first.name);
    else
      fprintf(outf,"\tcopyright: %s \"%s\" (rev. %ld)\n", cii->second.get_str(),
          cii->first.name, cii->first.rev);
  }
  
  fprintf(outf,"\tname: \"%s\" at \"%s\"", this->name,
      (this->fsptr != NULL ? this->fsptr->full_name : ""));
  fprintf(outf,"\n");
  fprintf(outf,"\ttitle: \"%s\"\n", this->title);
  if (this->selected)
    fprintf(outf,"\tselected: on (%ld)\n",this->selected_number);
  else
    fprintf(outf,"\tselected: off (%ld)\n",this->selected_number);
  
  thdb_revision_set_type::iterator rii;
  unsigned long rr;
  for (rr = 0; rr <= this->revision; rr++) {
    rii = this->db->revision_set.find(threvision(this->id, rr));
    fprintf(outf,"\trevision %ld: %s [%ld]\n", rr, rii->srcf.name, rii->srcf.line);
  }

}


bool thdataobject::is_in_survey(thsurvey * psearch)
{
  if (psearch == NULL)
    return false;
    
  thsurvey * tmp;
  if (this->get_class_id() == TT_SURVEY_CMD) {
    tmp = (thsurvey *) this;
  } else {
    tmp = this->fsptr;
  }
  
  while (tmp != NULL) {
    if (tmp->id == psearch->id)
      return true;
    tmp = tmp->fsptr;
  }
  
  return false;
}


void thdataobject::start_insert()
{
}


