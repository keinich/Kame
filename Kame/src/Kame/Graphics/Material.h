#pragma once

#include <map>

#include <Kame/Math/Vector4.h>
#include <Kame/Core/References.h>
#include <Kame/Graphics/RenderApi/RenderProgram.h>

namespace Kame {

  class CommandList;
  class Texture;

  enum MaterialRootParameters {
    MatricesCB,         // ConstantBuffer<Mat> MatCB : register(b0);
    MaterialCB,         // ConstantBuffer<Material> MaterialCB : register( b0, space1 );
    LightPropertiesCB,  // ConstantBuffer<LightProperties> LightPropertiesCB : register( b1 );
    InstanceDataSlot,   // StructuredBuffer<InstanceData> g_InstanceData : register (t0, space1);
    PointLights,        // StructuredBuffer<PointLight> PointLights : register( t0 );
    SpotLights,         // StructuredBuffer<SpotLight> SpotLights : register( t1 );
    Textures,           // Texture2D DiffuseTexture : register( t2 );
    MaterialParameters,  // StructuredBuffer<MaterialParameters> materialParameters : register (t1, space1);
    NumRootParameters
  };

  struct KAME_API BaseMaterialParameters {
    BaseMaterialParameters(
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
    uint32_t          Padding[3];
    //----------------------------------- (16 byte boundary)
    // Total:                              16 * 5 = 80 bytes

    // Define some interesting materials.
    static const BaseMaterialParameters Red;
    static const BaseMaterialParameters Green;
    static const BaseMaterialParameters Blue;
    static const BaseMaterialParameters Cyan;
    static const BaseMaterialParameters Magenta;
    static const BaseMaterialParameters Yellow;
    static const BaseMaterialParameters White;
    static const BaseMaterialParameters Black;
    static const BaseMaterialParameters Emerald;
    static const BaseMaterialParameters Jade;
    static const BaseMaterialParameters Obsidian;
    static const BaseMaterialParameters Pearl;
    static const BaseMaterialParameters Ruby;
    static const BaseMaterialParameters Turquoise;
    static const BaseMaterialParameters Brass;
    static const BaseMaterialParameters Bronze;
    static const BaseMaterialParameters Chrome;
    static const BaseMaterialParameters Copper;
    static const BaseMaterialParameters Gold;
    static const BaseMaterialParameters Silver;
    static const BaseMaterialParameters BlackPlastic;
    static const BaseMaterialParameters CyanPlastic;
    static const BaseMaterialParameters GreenPlastic;
    static const BaseMaterialParameters RedPlastic;
    static const BaseMaterialParameters WhitePlastic;
    static const BaseMaterialParameters YellowPlastic;
    static const BaseMaterialParameters BlackRubber;
    static const BaseMaterialParameters CyanRubber;
    static const BaseMaterialParameters GreenRubber;
    static const BaseMaterialParameters RedRubber;
    static const BaseMaterialParameters WhiteRubber;
    static const BaseMaterialParameters YellowRubber;
  };

  class MaterialBase {
  public:

    virtual void CreateProgram() = 0;
    inline RenderProgramSignature* GetSignature() const { return _ProgramSignature.get(); }
    inline RenderProgram* GetProgram() const { return _Program.get(); }

    MaterialBase();
    virtual ~MaterialBase();
  protected: //Fields

    NotCopyableReference<RenderProgramSignature> _ProgramSignature;
    NotCopyableReference<RenderProgram> _Program;

  };

  template<typename TParams>
  class Material : public MaterialBase {

    //friend class MaterialInstance<TParams>;

  public:

    virtual void CreateProgram() = 0;
    virtual void ApplyParameters(CommandList* commandList, std::vector<TParams>& params) {};

    //Reference<MaterialInstance<TParams>> CreateInstance();

  protected: //Methods
    Material<TParams>() {};
    virtual ~Material<TParams>() {};

  };

  class MaterialInstanceBase {
  public:
    virtual void ApplyParameters(CommandList* commandList) = 0;
    virtual MaterialBase* GetMaterial() = 0;
  };

  template<typename TParameters>
  class MaterialInstance : public MaterialInstanceBase {

  public:

    static Reference<MaterialInstance<TParameters>> CreateFromMaterial(Material<TParameters>* material);

    template<class TMaterial>
    static Reference<MaterialInstance<TParameters>> CreateFromMaterial();

    //void SetParameters(TParameters& params);
    TParameters& GetParameters();
    virtual void ApplyParameters(CommandList* commandList) override;
    virtual MaterialBase* GetMaterial() override;

    ~MaterialInstance<TParameters>();
  protected:
    MaterialInstance<TParameters>();

  private:

    static std::vector<TParameters> _ParameterCollection;
    uint16_t _Index;
    Material<TParameters>* _BaseMaterial;
    //TParameters _Parameters;

  };

  template<typename TParameters>
  std::vector<TParameters> MaterialInstance<TParameters>::_ParameterCollection;

  class MaterialInstanceCollectionBase {
  public:
    MaterialInstanceCollectionBase() {};
    virtual ~MaterialInstanceCollectionBase() {};
  };

  template <typename TParameters>
  class MaterialInstanceCollection {
  public:
    MaterialInstanceCollection<TParameters>() {}
    virtual ~MaterialInstanceCollection<TParameters>() { _ParemeterCollection.clear(); }

    void AddParameters(TParameters& parameters);

  private:
    std::vector<TParameters&> _ParemeterCollection;
  };

  template<typename TParameters>
  inline Reference<MaterialInstance<TParameters>> MaterialInstance<TParameters>::CreateFromMaterial(
    Material<TParameters>* material
  ) {
    MaterialInstance<TParameters>* ret1 = new MaterialInstance<TParameters>();
    auto ret = Reference< MaterialInstance<TParameters>>(ret1);
    
    ret->_BaseMaterial = material;
    return ret;
  }

  template<typename TParameters>
  inline TParameters& MaterialInstance<TParameters>::GetParameters() {
    return _ParameterCollection[_Index];
  }

  template<typename TParameters>
  inline void MaterialInstance<TParameters>::ApplyParameters(CommandList* commandList) {
    //_BaseMaterial->ApplyParameters(commandList, _ParameterCollection[_Index]);
    _BaseMaterial->ApplyParameters(commandList, _ParameterCollection);
  }

  template<typename TParameters>
  MaterialBase* MaterialInstance<TParameters>::GetMaterial() {
    return _BaseMaterial;
  }

  template<typename TParameters>
  template<class TMaterial>
  inline Reference<MaterialInstance<TParameters>> MaterialInstance<TParameters>::CreateFromMaterial() {
    MaterialInstance<TParameters>* ret1 = new MaterialInstance<TParameters>();

    auto ret = Reference< MaterialInstance<TParameters>>(ret1);

    Material<TParameters>* material = MaterialManager::GetMaterial<TMaterial>();
    ret->_BaseMaterial = material;
    return ret;
  }

  template<typename TParameters>
  inline MaterialInstance<TParameters>::~MaterialInstance() {
    _ParameterCollection.erase(_ParameterCollection.begin() + _Index);
  }

  template<typename TParameters>
  inline MaterialInstance<TParameters>::MaterialInstance() {
    _ParameterCollection.push_back(TParameters());
    _Index = _ParameterCollection.size() - 1;
  }

  template<typename TParameters>
  inline void MaterialInstanceCollection<TParameters>::AddParameters(TParameters& parameters) {
    _ParemeterCollection.push_back(parameters);
  }

}