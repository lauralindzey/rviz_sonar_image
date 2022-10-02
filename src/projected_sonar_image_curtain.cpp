#include <rviz_sonar_image/projected_sonar_image_curtain.h>
#include <rviz_sonar_image/color_map.h>

#include <rviz/ogre_helpers/mesh_shape.h>
#include <rviz/image/ros_image_texture.h>

#include <OGRE/OgreVector3.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgrePolygon.h>
#include <OGRE/OgreManualObject.h>


namespace rviz_sonar_image
{

ProjectedSonarImageCurtain::ProjectedSonarImageCurtain( Ogre::SceneManager* scene_manager, Ogre::SceneNode* parent_node, std::shared_ptr<ColorMap> color_map )
  :color_map_(color_map)
{
  mesh_shape_ = new rviz::MeshShape(scene_manager, parent_node);
  mesh_shape_->getMaterial()->getTechnique(0)->setLightingEnabled(false);
  Ogre::TextureUnitState* tu = mesh_shape_->getMaterial()->getTechnique(0)->getPass(0)->createTextureUnitState();
  tu->setTextureFiltering(Ogre::FT_MIN, Ogre::FO_ANISOTROPIC);
  tu->setTextureFiltering(Ogre::FT_MAG, Ogre::FO_POINT);
  tu->setTextureFiltering(Ogre::FT_MIP, Ogre::FO_POINT);
  tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP, Ogre::TextureUnitState::TAM_CLAMP, Ogre::TextureUnitState::TAM_CLAMP);
  texture_ = new rviz::ROSImageTexture();
  if (!texture_)
  {
    ROS_ERROR("Failed to create ROSImageTextures.");
  }

  mesh_shape_->setColor(1.0, 1.0, 1.0, 1.0);
}

ProjectedSonarImageCurtain::~ProjectedSonarImageCurtain()
{
  delete mesh_shape_;
  delete texture_;
}

void ProjectedSonarImageCurtain::addMessage(const acoustic_msgs::ProjectedSonarImage::ConstPtr& msg, uint32_t start_row, uint32_t end_row, int beam_number, const Ogre::Vector3& position, const Ogre::Quaternion& orientation )
{
  Ogre::Matrix4 transform;
  transform.makeTransform(position, Ogre::Vector3(1, 1, 1), orientation);

  mesh_shape_->clear();

  int max_steps = 128;
  int step_size = 1;
  start_row = std::min<uint32_t>(start_row, msg->ranges.size());
  end_row = std::min<uint32_t>(end_row, msg->ranges.size());
  if(!end_row>start_row)
    return;

  //double sample_length = (msg->ping_info.sound_speed/msg->sample_rate)/2.0;
  auto sample_count = end_row-start_row;
  auto steps = sample_count;

  if (steps > max_steps)
  {
    step_size = ceil(sample_count/float(max_steps));
    steps /= step_size;
  }

  // std::cerr << "step size: " << step_size << std::endl;
  // std::cerr << "sample count: " << sample_count << std::endl;
  // std::cerr << "sample length: " << sample_length << std::endl;
  // std::cerr << "steps: " << steps << std::endl;

  if(!image_)
  {
    image_ = sensor_msgs::Image::Ptr(new sensor_msgs::Image);
    image_->header.stamp = msg->header.stamp;
    image_->width = max_ping_count_;
    image_->height = sample_count;
    image_->encoding = "rgba8";
    image_->step = image_->width*4;
    image_->data.resize(image_->step*image_->height);
  }


  switch(msg->image.dtype)
  {
    case acoustic_msgs::SonarImageData::DTYPE_UINT8:
    {
      const uint8_t* sonar_data = reinterpret_cast<const uint8_t*>(msg->image.data.data());
      auto image_col = vertices_.size();

      for (uint32_t i = start_row; i < end_row; i++)
      {
        auto c = color_map_->lookup(sonar_data[i*msg->image.beam_count+beam_number]);
        auto image_row = i-start_row;
        auto image_cell = &image_->data.at((image_col+max_ping_count_*image_row)*4);
        image_cell[0] = c.r*255;
        image_cell[1] = c.g*255;
        image_cell[2] = c.b*255;
        image_cell[3] = c.a*255;
      }
      break;
    }
    case acoustic_msgs::SonarImageData::DTYPE_UINT16:
    {
      const uint16_t* sonar_data = reinterpret_cast<const uint16_t*>(msg->image.data.data());
      auto image_col = vertices_.size();

      for (uint32_t i = start_row; i < end_row; i++)
      {
        auto c = color_map_->lookup(sonar_data[i*msg->image.beam_count+beam_number]);
        auto image_row = i-start_row;
        auto image_cell = &image_->data.at((image_col+max_ping_count_*image_row)*4);
        image_cell[0] = c.r*255;
        image_cell[1] = c.g*255;
        image_cell[2] = c.b*255;
        image_cell[3] = c.a*255;
      }
      break;
    }
    case acoustic_msgs::SonarImageData::DTYPE_UINT32:
    {
      const uint32_t* sonar_data = reinterpret_cast<const uint32_t*>(msg->image.data.data());
      auto image_col = vertices_.size();

      for (uint32_t i = start_row; i < end_row; i++)
      {
        auto c = color_map_->lookup(sonar_data[i*msg->image.beam_count+beam_number]);
        auto image_row = i-start_row;
        auto image_cell = &image_->data.at((image_col+max_ping_count_*image_row)*4);
        image_cell[0] = c.r*255;
        image_cell[1] = c.g*255;
        image_cell[2] = c.b*255;
        image_cell[3] = c.a*255;
      }
      break;
    }
    case acoustic_msgs::SonarImageData::DTYPE_FLOAT32:
    {

      const float* sonar_data = reinterpret_cast<const float*>(msg->image.data.data());
      auto image_col = vertices_.size();

      for (uint32_t i = start_row; i < end_row; i++)
      {
        auto c = color_map_->lookup(sonar_data[i*msg->image.beam_count+beam_number]);
        auto image_row = i-start_row;
        auto image_cell = &image_->data.at((image_col+max_ping_count_*image_row)*4);
        image_cell[0] = c.r*255;
        image_cell[1] = c.g*255;
        image_cell[2] = c.b*255;
        image_cell[3] = c.a*255;
      }
      break;
    }
    default:
      ROS_WARN_STREAM("Unimplemented type: " << msg->image.dtype);
  }

  texture_->addMessage(image_);
  texture_->update();


  vertices_.resize(vertices_.size()+1);

  auto i = start_row;
  while(i < end_row)
  {
    float range = msg->ranges[i];
    vertices_.back().push_back(transform.transformAffine(Ogre::Vector3(msg->beam_directions[beam_number].x*range, msg->beam_directions[beam_number].y*range, msg->beam_directions[beam_number].z*range)));
    if(texture_coordinates_.size() < vertices_.back().size())
      texture_coordinates_.push_back((i-start_row)/float(end_row-start_row-1));
    if(i != end_row-1 && i+step_size >= end_row)
      i = end_row-1;
    else
      i+=step_size;
  }
  // std::cerr << vertices_.back().size() << " vertices added" << std::endl;

  // std::cerr << vertices_.size()*vertices_.front().size() << " total vertices" << std::endl;

  mesh_shape_->estimateVertexCount(vertices_.size()*vertices_.front().size());
  mesh_shape_->beginTriangles();

  for(int i = 0; i < vertices_.size(); i++)
  {
    float u = i/(max_ping_count_-1.0);
    for(int j = 0; j < vertices_[i].size(); j++)
    {
      mesh_shape_->addVertex(vertices_[i][j]);
      mesh_shape_->getManualObject()->textureCoord(u, texture_coordinates_[j]);
    }
  }

  int c1 = 0;
  for(int i = 0; i < vertices_.size()-1; i++)
  {
    int c2 = c1 + vertices_[i].size();
    for(int j = 0; j < vertices_[i].size()-1; j++)
    {
      mesh_shape_->addTriangle(c1+j, c1+j+1, c2+j+1);
      mesh_shape_->addTriangle(c2+j+1, c2+j, c1+j);
    }
    c1 = c2;
  }
  // std::cerr << "last triangle index: " << c1 + vertices_.back().size()-1 << std::endl;

  mesh_shape_->endTriangles();

  Ogre::Pass* pass = mesh_shape_->getMaterial()->getTechnique(0)->getPass(0);
  if (!pass)
  {
    ROS_ERROR("setMessage(): pass is NULL.");
    return;
  }

  if (pass->getNumTextureUnitStates() < 1)
  {
    ROS_ERROR("setMessage(): Number of texture unit states is less than 1.");
    return;
  }

  Ogre::TextureUnitState* unit_state = pass->getTextureUnitState(0);
  if (!unit_state)
  {
    ROS_ERROR("Failed to getTextureUnitState(%d).", 0);
    return;
  }

  unit_state->setTexture(texture_->getTexture());

}


bool ProjectedSonarImageCurtain::full() const
{
  return vertices_.size() >= max_ping_count_;
}

} // namespace rviz_sonar_image
