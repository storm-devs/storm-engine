#pragma once

#include "..//common_h//attributes.h"

class CXPICTURES
{
  public:
    CXPICTURES();
    ~CXPICTURES();

    void SetPictures(ATTRIBUTES *pAttributes);
    void ChangePicture(char *sAttributeName, char *sNewImageName);
    void ReleaseAllPictures();
};
