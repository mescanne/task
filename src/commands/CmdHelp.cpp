////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2006 - 2017, Paul Beckingham, Federico Hernandez.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// http://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#include <cmake.h>
#include <CmdHelp.h>
#include <algorithm>
#include <Table.h>
#include <Context.h>
#include <i18n.h>
#include <shared.h>
#include <format.h>
#include <util.h>



////////////////////////////////////////////////////////////////////////////////
CmdHelp::CmdHelp ()
{
  _keyword               = "help";
  _usage                 = "task          help ['usage']";
  _description           = STRING_CMD_HELP_USAGE;
  _read_only             = true;
  _displays_id           = false;
  _needs_gc              = false;
  _uses_context          = false;
  _accepts_filter        = false;
  _accepts_modifications = false;
  _accepts_miscellaneous = true;
  _category              = Command::Category::misc;
}

////////////////////////////////////////////////////////////////////////////////
int CmdHelp::execute (std::string& output)
{
  auto words = Context::getContext().cli2.getWords ();
  if (words.size () == 1 && closeEnough ("usage", words[0]))
    output = '\n'
           + composeUsage ()
           + '\n';
  else
    output = '\n'
           + composeUsage ()
           + '\n'
           + STRING_CMD_HELP_TEXT;

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
std::string CmdHelp::composeUsage () const
{
  Table view;
  view.width (Context::getContext().getWidth ());
  view.add ("");
  view.add ("");
  view.add ("");

  // Static first row.
  auto row = view.addRow ();
  view.set (row, 0, STRING_CMD_HELP_USAGE_LABEL);
  view.set (row, 1, "task");
  view.set (row, 2, STRING_CMD_HELP_USAGE_DESC);

  // Obsolete method of getting a list of all commands.
  std::vector <std::string> all;
  for (auto& cmd : Context::getContext().commands)
    all.push_back (cmd.first);

  // Sort alphabetically by usage.
  std::sort (all.begin (), all.end ());

  // Add the regular commands.
  for (auto& name : all)
  {
    if (name[0] != '_')
    {
      row = view.addRow ();
      view.set (row, 1, Context::getContext().commands[name]->usage ());
      view.set (row, 2, Context::getContext().commands[name]->description ());
    }
  }

  // Add the helper commands.
  for (auto& name : all)
  {
    if (name[0] == '_')
    {
      row = view.addRow ();
      view.set (row, 1, Context::getContext().commands[name]->usage ());
      view.set (row, 2, Context::getContext().commands[name]->description ());
    }
  }

  // Add the aliases commands.
  row = view.addRow ();
  view.set (row, 1, " ");

  for (auto& alias : Context::getContext().config)
  {
    if (alias.first.substr (0, 6) == "alias.")
    {
      row = view.addRow ();
      view.set (row, 1, alias.first.substr (6));
      view.set (row, 2, format (STRING_CMD_HELP_ALIASED, alias.second));
    }
  }

  return view.render ();
}

////////////////////////////////////////////////////////////////////////////////
