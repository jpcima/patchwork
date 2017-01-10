#include "pwareascene.h"
#include "pwnode.h"
#include "pwconnector.h"
#include <QDebug>
#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>
#include <boost/scope_exit.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

void PwAreaScene::autoLayout() {
  auto NC = [](const char *p) -> char * { return const_cast<char *>(p); };
  auto pt2in = [](double v) -> double { return v / 72; };
  auto in2pt = [](double v) -> double { return v * 72; };

  const unsigned hspacing = 16;
  const unsigned vspacing = 0;

  GVC_t *gvc = gvContext();
  BOOST_SCOPE_EXIT(gvc) { gvFreeContext(gvc); } BOOST_SCOPE_EXIT_END;
  Agraph_t *g = agopen(NC("G"), Agdirected, 0);
  BOOST_SCOPE_EXIT(g) { agclose(g); } BOOST_SCOPE_EXIT_END;

  agattr(g, AGRAPH, NC("splines"), NC("line"));
  agattr(g, AGNODE, NC("shape"), NC("box"));

  Agsym_t *sym_fixedsize = agattr(g, AGNODE, NC("fixedsize"), NC(""));
  Agsym_t *sym_width = agattr(g, AGNODE, NC("width"), NC(""));
  Agsym_t *sym_height = agattr(g, AGNODE, NC("height"), NC(""));
  Agsym_t *sym_tailport = agattr(g, AGEDGE, NC("tailport"), NC(""));
  Agsym_t *sym_headport = agattr(g, AGEDGE, NC("headport"), NC(""));

  for (PwNode *node : this->nodes()) {
    std::string name = node->id().toStdString();
    QRectF bounds = node->boundingRect();
    double gw = pt2in(bounds.width() + hspacing);
    double gh = pt2in(bounds.height() + vspacing);
    Agnode_t *gn = agnode(g, NC(name.c_str()), true);
    agxset(gn, sym_fixedsize, NC("true"));
    agxset(gn, sym_width, NC(std::to_string(gw).c_str()));
    agxset(gn, sym_height, NC(std::to_string(gh).c_str()));
  }

  auto endpoint_port = [](PwEndpoint *ep) -> const char * {
    switch (ep->placement()) {
      case PwEndpoint::TopPlacement: return "n";
      case PwEndpoint::BottomPlacement: return "s";
      case PwEndpoint::LeftPlacement: return "w";
      case PwEndpoint::RightPlacement: return "e";
      default: Q_ASSERT(false);
    }
  };

  for (PwConnector *conn : this->connectors()) {
    PwEndpoint *epsrc = conn->sourceEndpoint();
    PwEndpoint *epdst = conn->targetEndpoint();
    std::string srcname = epsrc->parentNode()->id().toStdString();
    std::string dstname = epdst->parentNode()->id().toStdString();
    Agnode_t *gs = agnode(g, NC(srcname.c_str()), false);
    Agnode_t *gt = agnode(g, NC(dstname.c_str()), false);
    Agedge_t *e = agedge(g, gs, gt, NC(""), true);
    agxset(e, sym_tailport, NC(endpoint_port(epsrc)));
    agxset(e, sym_headport, NC(endpoint_port(epdst)));
  }

  if (0)
    agwrite(g, stdout);

  gvLayout(gvc, g, NC("dot"));

  double minx = HUGE_VAL;
  double maxy = 0;
  for (Agnode_t *gn = agfstnode(g); gn; gn = agnxtnode(g, gn)) {
    pointf gp = ND_coord(gn);
    double gh = in2pt(boost::lexical_cast<double>(agxget(gn, sym_height)));
    minx = std::min(gp.x, minx);
    maxy = std::max(gp.y + gh, maxy);
  }

  for (PwNode *node : this->nodes()) {
    std::string name = node->id().toStdString();
    Agnode_t *gn = agnode(g, NC(name.c_str()), false);
    pointf gp = ND_coord(gn);
    QPointF pt(gp.x - minx, maxy - gp.y);
    node->setPos(pt);
  }
}
