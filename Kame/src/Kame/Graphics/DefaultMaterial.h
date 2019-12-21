#pragma once

#include <vector>

#include "Material.h"

namespace Kame {

  struct DefaultMaterialParameters {

    DefaultMaterialParameters(
      Kame::Math::Float4 emissive = { 0.0f, 0.0f, 0.0f, 1.0f },
      Kame::Math::Float4 ambient = { 0.1f, 0.1f, 0.1f, 1.0f },
      Kame::Math::Float4 diffuse = { 1.0f, 1.0f, 1.0f, 1.0f },
      Kame::Math::Float4 specular = { 1.0f, 1.0f, 1.0f, 1.0f },
      float specularPower = 128.0f
    )
      : Emissive(emissive)
      , Ambient(ambient)
      , Diffuse(diffuse)
      , Specular(specular)
      , SpecularPower(specularPower) {}

    Kame::Math::Float4 Emissive;
    //----------------------------------- (16 byte boundary)
    Kame::Math::Float4 Ambient;
    //----------------------------------- (16 byte boundary)
    Kame::Math::Float4 Diffuse;
    //----------------------------------- (16 byte boundary)
    Kame::Math::Float4 Specular;
    //----------------------------------- (16 byte boundary)
    float             SpecularPower;
    uint32_t          TextureIndexForShader;
    uint32_t          TextureIndexStatic;
    uint32_t          Padding[1];
    //Texture* DiffuseTexture;

    void SetDiffuseTexture(Texture* texture);

    static void Clear() { _Textures.clear(); }

    static std::vector<Texture*> _Textures;

  };

  class DefaultMaterial : public Material<DefaultMaterialParameters> {

  public:

    //virtual void ApplyParameters(CommandList* commandList, DefaultMaterialParametersForShader& params) override;
    virtual void ApplyParameters(CommandList* commandList, std::vector<DefaultMaterialParameters>& params) override;
    virtual void ApplyParameters(
      CommandList* commandList,
      std::vector<MaterialInstanceBase*>& materialInstances
    ) override;

    virtual void CreateProgram() override;

    DefaultMaterial() { CreateProgram(); }
    virtual ~DefaultMaterial() {};

  protected:

  };

  inline MaterialInstance<DefaultMaterialParameters>::~MaterialInstance() {
    //_ParameterCollection.erase(_ParameterCollection.begin() + _Index);
    //if (_ParameterCollection.size() == 0) {
      //DefaultMaterialParameters::Clear();
    //}
  }

}