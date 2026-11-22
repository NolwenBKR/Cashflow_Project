#ifndef MODEL_H
#define MODEL_H

template <typename Derived>
class BaseModel
{
public:
    virtual ~BaseModel() = default;

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }
    bool isNull() const { return m_id <= 0; }

    static Derived null()
    {
        return Derived(); // m_id vaut -1 par defaut
    }

private:
    int m_id = -1;
};

#endif // MODEL_H
