
/*struct Shape
{
    std::vector<DirectX::XMFLOAT4> vertices;
    ID3D11Buffer* vertexBuffer = nullptr; // ”никальный буфер дл€ каждой фигуры
};

class TriangleComponent
{
public:
    TriangleComponent(ID3D11Device* device, ID3D11DeviceContext* context);
    void InitializeShape(DirectX::XMFLOAT4 points[], DirectX::XMFLOAT4 colors[], int count, const DirectX::XMFLOAT2& offset);
    void MoveShape(float dx, float dy, float dz);

    float MoveSpeed = 0.1f;
    float DirectionX = -1.0f;
    float DirectionY = 0.0f;
private:
    int vertexCount = 0; 

    ID3D11Device* device;
    ID3D11DeviceContext* context;

    void CreateVertexBuffer(DirectX::XMFLOAT4 points[], int count, const DirectX::XMFLOAT2& offset);
    void CreateIndexBuffer();
};*/
