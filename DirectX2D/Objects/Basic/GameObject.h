#pragma once

class GameObject : public Transform
{
public:
    GameObject(wstring shaderFile = L"UV.hlsl");
    virtual ~GameObject() override;

    void SetRender();

    void SetPixelShader(wstring shaderFile);
    
protected:
    VertexShader* vertexShader;
    PixelShader* pixelShader;    
    MatrixBuffer* worldBuffer;
};