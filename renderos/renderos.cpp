/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2005 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using Coin with software that can not be combined with the GNU
 *  GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See <URL:http://www.coin3d.org/> for more information.
 *
 *  Systems in Motion, Postboks 1283, Pirsenteret, 7462 Trondheim, NORWAY.
 *  <URL:http://www.sim.no/>.
 *
\**************************************************************************/

#include <stdio.h>
#include <assert.h>

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/SoOffscreenRenderer.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>

#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#if HAVE_UNISTD_H
#include <unistd.h> // getopt()
#endif /* HAVE_UNISTD_H */

#ifdef HAVE_SMALLCHANGE_LIBRARY
#include <SmallChange/misc/Init.h>
#endif // HAVE_SMALLCHANGE_LIBRARY


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
  fprintf(stderr, "\nUsage: %s [-h -v] [-x width] [-y height] [-c components] <modelfile.iv> <snapshot.rgb>\n\n", invname);
  fprintf(stderr, "\t-h:\tshow usage\n");
  fprintf(stderr, "\t-v:\tcall ''view all'' on camera before snapshot\n");
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

  SbBool viewall = FALSE;

#ifdef HAVE_GETOPT
  /* Parse command line. */
  int getoptchar;
  while ((getoptchar = getopt(argc, argv, "?hvx:y:c:t:")) != EOF) {
    switch (getoptchar) {
    case '?':
    case ':':
    case 'h':
      usage(argv[0]);
      exit(0);
      break;
    case 'v':
      viewall = TRUE;
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

#ifdef HAVE_SMALLCHANGE_LIBRARY
  smallchange_init();
#endif // HAVE_SMALLCHANGE_LIBRARY

  SbViewportRegion vp(width, height);

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

  root->addChild(fileroot);

  // If there's no camera in the scene graph already, set up a "super
  // graph" above the one loaded from file, and add a camera.

  // The camera and light searches should extend into nodekits.
  (void)SoBaseKit::setSearchingChildren(TRUE);

  SoSearchAction searchaction;
  searchaction.setType(SoCamera::getClassTypeId());
  searchaction.setInterest(SoSearchAction::FIRST);
  searchaction.apply(root);

  SoCamera * camera = NULL;
  SoPath * p = searchaction.getPath();

  if (p == NULL) {
    SoDebugError::postInfo("main",
                           "Found no camera in scene, so a default "
                           "perspective camera in a \"view all\" "
                           "position will be set up.");

    camera = new SoPerspectiveCamera;
    camera->viewAll(root, vp);
    root->insertChild(camera, 0);
  }
  else {
    SoNode * n = ((SoFullPath *)p)->getTail();
    assert(n && n->getTypeId().isDerivedFrom(SoCamera::getClassTypeId()));
    camera = (SoCamera *)n;
  }

  if (viewall) { camera->viewAll(root, vp); }

  // Add in a (head) light aswell, if there was none in the scene.

  searchaction.reset();
  searchaction.setType(SoLight::getClassTypeId());
  searchaction.setInterest(SoSearchAction::FIRST);
  searchaction.apply(fileroot);

  if (searchaction.getPath() == NULL) {
    SoDebugError::postInfo("main",
                           "Found no light sources in scene, so a "
                           "default will be set up.");

    SoDirectionalLight * light = new SoDirectionalLight;
    light->direction.setValue(0.25, 0.25, -0.8);
    root->insertChild(light, 0);
  }

  SoOffscreenRenderer osr(vp);
  osr.setComponents(components);
  osr.setBackgroundColor(SbColor(0.2f, 0.4f, 0.6f));

  SoGLRenderAction * glra = osr.getGLRenderAction();
  glra->setTransparencyType(transtype);

  SbBool wasrendered = osr.render(root);
  root->unref();

  if (!wasrendered) {
    (void)fprintf(stderr, "Couldn't render buffer (functionality not available?).\n");
    exit(1);
  }

  SbString filename(snapshotdump);
  if (!osr.writeToRGB(filename.getString())) {
    (void)fprintf(stderr, "Couldn't write file '%s'.\n", snapshotdump);
  }
  else {
    (void)fprintf(stdout, "Bitmap file in SGI RGB format written to '%s'.\n",
                  snapshotdump);
  }

  return 0;
}
