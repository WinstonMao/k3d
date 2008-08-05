#include "PreviewCube.h"

namespace module
{
namespace ngui
{
namespace material_manager
{
namespace mechanics
{


void PreviewCube::init(k3d::string_t _node_name, k3d::string_t _meta_nametag)
{
  m_doc_node 
    = dynamic_cast<k3d::inode*>(k3d::plugin::create("Torus", 
                                                    m_document_state->document(), 
                                                    _node_name));


  k3d::property::set_internal_value(*m_doc_node, 
                                    "render_shadows", false);
  
  k3d::property::set_internal_value(*m_doc_node, 
                                    "viewport_visible", false);

  //Create Meta Data
  if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(m_doc_node))
    metadata->set_metadata("PreviewCore::nametag", _meta_nametag);
}


}//namespace mechanics

}//namespace material_manager

}//namespace ngui

}//namespace module