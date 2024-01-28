#include "shadow_map.h"

#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

ShadowMapUPtr ShadowMap::Create(int width, int height) {
    auto shadowMap = ShadowMapUPtr(new ShadowMap());
    if (!shadowMap->Init(width, height))
        return nullptr;
    return std::move(shadowMap);
}

ShadowMap::~ShadowMap() {
    if (m_framebuffer) {
        glDeleteFramebuffers(1, &m_framebuffer);
    }
}


void ShadowMap::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

bool ShadowMap::Init(int width, int height) {

    m_width = width;
    m_height = height;

    glGenFramebuffers(1, &m_framebuffer);
    Bind();

    m_shadowMap = Texture::Create(width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
    m_shadowMap->SetFilter(GL_NEAREST, GL_NEAREST);
    m_shadowMap->SetWrap(GL_REPEAT, GL_REPEAT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D, m_shadowMap->Get(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        SPDLOG_ERROR("failed to complete shadow map framebuffer: {:x}", status);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void ShadowMap::Save()
{
    //TexturePtr shadowMap = m_shadowMap->GetShadowMap();
    Bind();

        /// READ THE PIXELS VALUES from FBO AND SAVE TO A .PPM FILE
    int             i, j, k;
    float   *pixels = (float*)malloc(m_width*m_height*sizeof(float));
    unsigned char  *img  = (unsigned char*)malloc(m_width*m_height*sizeof(unsigned char)); 

    /// READ THE CONTENT FROM THE FBO
    glReadBuffer(GL_DEPTH_ATTACHMENT);
    glReadPixels(0, 0, m_width, m_height, GL_DEPTH_COMPONENT, GL_FLOAT, pixels);

    k = 0;
    for(i=0; i<m_height; i++)
    {
        for(j=0; j<m_width; j++)
        {
            
            img[m_width*i+j] = 255*pixels[m_width*i+j];
        }
        //fprintf(output_image,"\n");
    }

    stbi_write_bmp("shadowMap.bmp", m_width, m_height, 1, img);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    free(pixels);
    free(img);
}