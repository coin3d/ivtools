/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2000 by Systems in Motion. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  version 2.1 as published by the Free Software Foundation. See the
 *  file LICENSE.LGPL at the root directory of the distribution for
 *  more details.
 *
 *  If you want to use Coin for applications not compatible with the
 *  LGPL, please contact SIM to acquire a Professional Edition license.
 *
 *  Systems in Motion, Prof Brochs gate 6, 7030 Trondheim, NORWAY
 *  http://www.sim.no support@sim.no Voice: +47 22114160 Fax: +47 22207097
 *
\**************************************************************************/

#include <stdio.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/nodes/SoSeparator.h>

// FIXME: do proper checks for these through configure.
// 19991206 mortene.
#ifndef _WIN32
#define HAVE_GETOPT 1
#define HAVE_ISATTY 1
#endif // !_WIN32

#if HAVE_GETOPT
#include <unistd.h>
/* These two externs are for interfacing against getopt(). */
extern int optind;
extern char * optarg;
#endif // HAVE_GETOPT


#if HAVE_GETOPT
void
usage(const char * argv_0)
{
  fprintf(stderr, "\nUsage: %s [-bfth] [-o filename] file ...\n", argv_0);

  fprintf(stderr, "\t-b:\twrite in binary format\n");
  fprintf(stderr, "\t-f:\tflatten SoFile nodes\n");
  fprintf(stderr, "\t-t:\tinline all textures\n");
  fprintf(stderr, "\t-h:\tshow usage\n");
  fprintf(stderr, "\t-o:\twrite to ``filename'' instead of stdout\n");

  fprintf(stderr,
          "\n  ``%s'' will read all files given on the command line,\n",
          argv_0);
  fprintf(stderr,
          "  compile them into a single scene and write them back out\n");
  fprintf(stderr,
          "  again on stdout (unless the ``-o'' option is used).\n\n");
  fprintf(stderr,
          "  Use ``-'' or no input files to read from stdin.\n\n");
}
#endif // !HAVE_GETOPT


#if HAVE_ISATTY
#define CHECK_TTY(fileptr) \
  do { \
    if (isatty(fileno(fileptr))) { \
      usage(argv[0]); \
      fprintf(stderr, "Trying to read from standard input, but standard " \
              "input is a tty!\n\n"); \
      exit(1); \
    } \
  } while (0)
#else // !HAVE_ISATTY
#define CHECK_TTY(fileptr) do { } while (0)
#endif // !HAVE_ISATTY



int
main(int argc, char * argv[])
{
  int writeasbinary = 0;
  int flattenfiles = 0;
  int flattentextures = 0;
  const char * outname = NULL;

#if HAVE_GETOPT
  /* Parse command line. */
  int getoptchar;
  while ((getoptchar = getopt(argc, argv, "bftho:")) != EOF) {
    char c = (char)getoptchar;
    switch (c) {
    case 'b':
      writeasbinary = 1;
      break;
    case 'f':
      flattenfiles = 1;
      break;
    case 't':
      flattentextures = 1;
      break;
    case 'h':
      usage(argv[0]);
      exit(0);
      break;
    case 'o':
      outname = optarg;
      if (!outname) {
        usage(argv[0]);
        exit(1);
      }
      break;
    case '?':
      usage(argv[0]);
      exit(1);
      break;
    }
  }
  int i = optind;
#else // !HAVE_GETOPT
  int i = 1;
#endif // !HAVE_GETOPT

  SoDB::init();
  SoNodeKit::init();
  SoInteraction::init();

  SoInput stdinp;
  SoSeparator * root = new SoSeparator;

  if (i >= argc) {
    CHECK_TTY(stdin);
    /* No files specified, read from stdin only. */
    SoSeparator * tmp = SoDB::readAll(&stdinp);
    if (tmp) root->addChild(tmp);
    else {
      fprintf(stderr, "Error: not a valid file on stdin.\n");
    }
  }
  else {
    SoInput fileinp;

    for (; i < argc; i++) {
      SoInput * inp = NULL;

      if (strcmp("-", argv[i]) == 0) {
        CHECK_TTY(stdinp.getCurFile());
        inp = &stdinp;
      }
      else if (fileinp.openFile(argv[i])) {
        inp = &fileinp;
      }
      else {
        fprintf(stderr, "Couldn't open file '%s' -- skipping.\n", argv[i]);
      }

      if (inp) {
        SoSeparator * tmproot = SoDB::readAll(inp);
        if (tmproot) {
          root->addChild(tmproot->getNumChildren() == 1 ?
                         tmproot->getChild(0) : tmproot);
        }
        else {
          fprintf(stderr, "Couldn't read file '%s' -- skipping.\n", argv[i]);
        }

        if (inp == &fileinp) inp->closeFile();
      }
    }
  }

  if (flattenfiles) {
    // FIXME: flatten SoFile nodes. 19991009 mortene.
    fprintf(stderr, "Warning: SoFile flattening not supported yet!\n");
  }

  if (flattentextures) {
    // FIXME: inline SoTexture2 nodes. 19991009 mortene.
    fprintf(stderr, "Warning: texture inlining not supported yet!\n");
  }

  if (root->getNumChildren() > 0) {
    SoOutput out;
    if (outname) {
      if (!out.openFile(outname)) {
        fprintf(stderr, "Couldn't write output to '%s'.\n", outname);
        exit(1);
      }
    }
    out.setBinary(writeasbinary);

    SoWriteAction wa(&out);
    for (int i=0; i < root->getNumChildren(); i++) {
      wa.apply(root->getChild(i));
    }
  }

  return 0;
}
