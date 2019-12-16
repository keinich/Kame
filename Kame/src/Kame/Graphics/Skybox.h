#pragma once

#include "Material.h"
#include <Kame/Core/References.h>

namespace Kame {

  class MaterialInstanceBase;
  class Mesh;
  class Texture;
  class CommandList;

  struct SkyboxMaterialParameters {
    Reference<Texture> CubeTexture;
  };

  class SkyboxMaterial : public Material<SkyboxMaterialParameters> {

  public:
    void ApplySkyboxParameters(CommandList* commandList, SkyboxMaterialParameters& params);
    virtual void CreateProgram() override;

    SkyboxMaterial() { CreateProgram(); }
    virtual ~SkyboxMaterial() {};
  };

  class Skybox {

  public:
    Skybox();
    virtual ~Skybox() {};

    void SetTexture(Texture* texture);
    inline RenderProgram* GetProgram() { return _MaterialInstance->GetMaterial()->GetProgram(); }

    void Draw(CommandList* commandList);

  private:
    Reference<MaterialInstance<SkyboxMaterialParameters>> _MaterialInstance;
    SkyboxMaterial* _Material;
    Mesh* _Mesh;
  };

}