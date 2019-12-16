#pragma once

#include <map>

#include <Kame/Math/Vector4.h>
#include <Kame/Core/References.h>
#include <Kame/Graphics/RenderApi/RenderProgram.h>

namespace Kame {

  class CommandList;
  class Texture;

  enum DefaultMaterialRootParameters {
    MatricesCB1,         // ConstantBuffer<Mat> MatCB : register(b0);
    MaterialCB1,         // ConstantBuffer<Material> MaterialCB : register( b0, space1 );
    LightPropertiesCB1,  // ConstantBuffer<LightProperties> LightPropertiesCB : register( b1 );
    InstanceData1,        // StructuredBuffer<InstanceData> g_InstanceData : register (t0, space1);
    PointLights1,        // StructuredBuffer<PointLight> PointLights : register( t0 );
    SpotLights1,         // StructuredBuffer<SpotLight> SpotLights : register( t1 );
    Textures1,           // Texture2D DiffuseTexture : register( t2 );
    MaterialParameters,  // StructuredBuffer<MaterialParameters> materialParameters : register (t1, space1);
    NumRootParameters1
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

  struct DefaultMaterialParameters {
    BaseMaterialParameters BaseParams;
    Texture* DiffuseTexture;
  };

  struct DefaultMaterialParametersForShader {

    DefaultMaterialParametersForShader(
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
    uint32_t          TextureIndex;
    uint32_t          Padding[2];

    void SetDiffuseTexture(Texture* texture);

    static void Clear() { _Textures.clear(); }

    static std::vector<Texture*> _Textures;
  
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
    virtual void ApplyParameters(CommandList* commandList, TParams& params) {};
    virtual void ApplyParameters1(CommandList* commandList, std::vector<TParams>& params) {};

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

    //template<typename T>
    //friend class std::shared_ptr<MaterialInstance<TParameters>>;    

  public:

    static Reference<MaterialInstance<TParameters>> CreateFromMaterial(Material<TParameters>* material);

    template<class TMaterial>
    static Reference<MaterialInstance<TParameters>> CreateFromMaterial1();

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

  class DefaultMaterial : public Material<DefaultMaterialParametersForShader> {

  public:


    //virtual void ApplyParameters(CommandList* commandList, DefaultMaterialParametersForShader& params) override;
    virtual void ApplyParameters1(CommandList* commandList, std::vector<DefaultMaterialParametersForShader>& params) override;


    virtual void CreateProgram() override;

    DefaultMaterial() { CreateProgram(); }
    virtual ~DefaultMaterial() {};
  protected:

    //DefaultMaterialParameters _Parameters;
    //Texture* _DiffuseTexture;
  };

  template<typename TParameters>
  inline Reference<MaterialInstance<TParameters>> MaterialInstance<TParameters>::CreateFromMaterial(
    Material<TParameters>* material
  ) {
    auto ret = CreateReference< MaterialInstance<TParameters>>();
    ret->_BaseMaterial = material;
    return ret;
  }

  //template<typename TParameters>
  //inline void MaterialInstance<TParameters>::SetParameters(TParameters& params) {
  //  _Parameters = params;
  //}

  template<typename TParameters>
  inline TParameters& MaterialInstance<TParameters>::GetParameters() {
    return _ParameterCollection[_Index];
  }

  template<typename TParameters>
  inline void MaterialInstance<TParameters>::ApplyParameters(CommandList* commandList) {
    _BaseMaterial->ApplyParameters(commandList, _ParameterCollection[_Index]);
    _BaseMaterial->ApplyParameters1(commandList, _ParameterCollection);
  }

  template<typename TParameters>
  MaterialBase* MaterialInstance<TParameters>::GetMaterial() {
    return _BaseMaterial;
  }

  template<typename TParameters>
  template<class TMaterial>
  inline Reference<MaterialInstance<TParameters>> MaterialInstance<TParameters>::CreateFromMaterial1() {
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

  inline MaterialInstance<DefaultMaterialParametersForShader>::~MaterialInstance() {
    _ParameterCollection.erase(_ParameterCollection.begin() + _Index);
    if (_ParameterCollection.size() == 0) {
      DefaultMaterialParametersForShader::Clear();
    }
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