#ifndef RVIZ_SONAR_IMAGE_SONAR_IMAGE_CURTAIN_H
#define RVIZ_SONAR_IMAGE_SONAR_IMAGE_CURTAIN_H

#include <acoustic_msgs/RawSonarImage.h>
#include <sensor_msgs/Image.h>

namespace Ogre
{
  class SceneManager;
  class SceneNode;
  class ManualObject;
  class Vector3;
  class Quaternion;
}

namespace rviz
{
  class MeshShape;
  class ROSImageTexture;
}

namespace rviz_sonar_image
{
class ColorMap;

class SonarImageCurtain
{
public:
  SonarImageCurtain( Ogre::SceneManager* scene_manager, Ogre::SceneNode* parent_node, std::shared_ptr<ColorMap> color_map);
  ~SonarImageCurtain();

  void addMessage(const acoustic_msgs::RawSonarImage::ConstPtr& msg, uint32_t start_row, uint32_t end_row, int beam_number, const Ogre::Vector3& position, const Ogre::Quaternion& orientation );

  bool full() const;

private:
  rviz::MeshShape* mesh_shape_;

  std::vector<std::vector<Ogre::Vector3> > vertices_;

  sensor_msgs::Image::Ptr image_;
  rviz::ROSImageTexture* texture_;
  std::shared_ptr<ColorMap> color_map_;

  int max_ping_count_=4096;
  int row_count_ = 0;
};

} // namespace rviz_sonar_image

#endif
