/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization project.
 *  Copyright (C) 1998-2000 by Systems in Motion. All rights reserved.
 *
 *  This application is Free Software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  version 2 as published by the Free Software Foundation. See the
 *  file LICENSE.GPL at the root directory of the distribution for
 *  more details.
 *
 *  If you want to use this code for applications not compatible with the
 *  GPL, please contact SIM to acquire a Coin Professional Edition license.
 *
 *  Systems in Motion, Prof Brochs gate 6, 7030 Trondheim, NORWAY
 *  http://www.sim.no support@sim.no Voice: +47 22114160 Fax: +47 22207097
 *
\**************************************************************************/

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/SoOffscreenRenderer.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <stdio.h>
#include <assert.h>

#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#if HAVE_UNISTD_H
#include <unistd.h> // getopt()
#endif /* HAVE_UNISTD_H */

#ifdef HAVE_GETOPT
/* These two externs are for interfacing against getopt(). */
extern int optind;
extern char * optarg;
#endif // HAVE_GETOPT


void
usage(const char * invname)
{
#ifdef HAVE_GETOPT
  // FIXME: the -r option is not activated yet (don't know how to have
  // multiple values on an option with getopt(). 20011024 mortene.
//    fprintf(stderr, "\nUsage: %s [-h] [-x width] [-y height] [-c components] [-r x y z a] <modelfile.iv> <snapshot.rgb>\n\n", invname);
  fprintf(stderr, "\nUsage: %s [-h] [-x width] [-y height] [-c components] <modelfile.iv> <snapshot.rgb>\n\n", invname);
  fprintf(stderr, "\t-h:\tshow usage\n");
  fprintf(stderr, "\t-x:\tset width in pixels (default 640)\n");
  fprintf(stderr, "\t-y:\tset height in pixels (default 480)\n");
  fprintf(stderr, "\t-c:\tset components in image\n");
  fprintf(stderr, "\t\t\t1 = LUMINANCE\n");
  fprintf(stderr, "\t\t\t2 = LUMINANCE with transparency\n");
  fprintf(stderr, "\t\t\t3 = RGB (default)\n");
  fprintf(stderr, "\t\t\t4 = RGB with transparency\n");
  fprintf(stderr, "\t-t:\ttransparency mode\n");
  fprintf(stderr, "\t\t\t0 = SCREEN_DOOR (default)\n");
  fprintf(stderr, "\t\t\t1 = ADD\n");
  fprintf(stderr, "\t\t\t2 = DELAYED_ADD\n");
  fprintf(stderr, "\t\t\t3 = SORTED_OBJECT_ADD\n");
  fprintf(stderr, "\t\t\t4 = BLEND\n");
  fprintf(stderr, "\t\t\t5 = DELAYED_BLEND\n");
  fprintf(stderr, "\t\t\t6 = SORTED_OBJECT_BLEND\n");
#ifdef __COIN__ // The next two are Coin extensions.
  fprintf(stderr, "\t\t\t7 = SORTED_OBJECT_SORTED_TRIANGLE_ADD\n");
  fprintf(stderr, "\t\t\t8 = SORTED_OBJECT_SORTED_TRIANGLE_BLEND\n");
#endif // __COIN__

  // FIXME: see FIXME above about "-r". 20011024 mortene.
//    fprintf(stderr, "\t-r:\tcamera rotation, axis plus angle (default none)\n");
  fprintf(stderr, "\n");
#else // !HAVE_GETOPT
  fprintf(stderr, "\nUsage: %s <modelfile.iv> <snapshot.rgb>\n\n", invname);
#endif // !HAVE_GETOPT
}

int
main(int argc, char ** argv)
{
  int width = 640;
  int height = 480;
  SoOffscreenRenderer::Components components = SoOffscreenRenderer::RGB;

  SoGLRenderAction::TransparencyType transtype = SoGLRenderAction::SCREEN_DOOR;
  SoGLRenderAction::TransparencyType maxtranstype = SoGLRenderAction::SORTED_OBJECT_BLEND;
#ifdef __COIN__ // The next two are Coin extensions.
  maxtranstype = SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_BLEND;
#endif // __COIN__


#ifdef HAVE_GETOPT
  /* Parse command line. */
  int getoptchar;
  while ((getoptchar = getopt(argc, argv, "?hx:y:c:t:")) != EOF) {
    switch (getoptchar) {
    case '?':
    case ':':
    case 'h':
      usage(argv[0]);
      exit(0);
      break;
    case 'x':
      width = atoi(optarg);
      break;
    case 'y':
      height = atoi(optarg);
      break;
    case 'c':
      components = (SoOffscreenRenderer::Components)atoi(optarg);
      if (components < 1 || components > 4) {
        (void)fprintf(stderr, "Invalid number of components: %d\n",
                      components);
        exit(1);
      }
      break;
    case 't':
      transtype = (SoGLRenderAction::TransparencyType)atoi(optarg);
      if (transtype < 0 || transtype > maxtranstype) {
        (void)fprintf(stderr, "Invalid transparency type %d\n", transtype);
        exit(1);
      }
      break;
    default:
      assert(FALSE);
      break;
    }
  }

  int i = optind;
#else // !HAVE_GETOPT
  int i = 1;
#endif // !HAVE_GETOPT

  if ((argc - i) != 2) {
    usage(argv[0]);
    exit(1);
  }

  const char * modelfile = argv[i++];
  const char * snapshotdump = argv[i++];

  SoDB::init();
  SoNodeKit::init();
  SoInteraction::init();

  SoInput in;
  if (!in.openFile(modelfile)) {
    (void)fprintf(stderr, "Couldn't open file '%s'.\n", modelfile);
    exit(1);
  }

  SoSeparator * fileroot = SoDB::readAll(&in);
  if (!fileroot) {
    (void)fprintf(stderr, "Couldn't read file '%s'.\n", modelfile);
    exit(1);
  }

  SoSeparator * root = new SoSeparator;
  root->ref();

  SoPerspectiveCamera * camera = new SoPerspectiveCamera;
  root->addChild(camera);

  SoDirectionalLight * light = new SoDirectionalLight;
  light->direction.setValue(0.5, 0.5, -0.8);
  root->addChild(light);

  root->addChild(fileroot);

  SbViewportRegion vp(width, height);
  camera->viewAll(root, vp);

  SoOffscreenRenderer osr(vp);
  osr.setComponents(components);

  SoGLRenderAction * glra = osr.getGLRenderAction();
  glra->setTransparencyType(transtype);

  SbBool wasrendered = osr.render(root);
  root->unref();

  if (!wasrendered) {
    (void)fprintf(stderr, "Couldn't render buffer (functionality not available?).\n");
    exit(1);
  }

  if (!osr.writeToRGB(snapshotdump)) {
    (void)fprintf(stderr, "Couldn't write file '%s'.\n", snapshotdump);
  }
  else {
    (void)fprintf(stdout, "Bitmap file in SGI RGB format written to '%s'.\n",
                  snapshotdump);
  }

  return 0;
}
