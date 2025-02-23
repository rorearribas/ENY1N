#pragma once
#include <algorithm>
#include <array>

namespace utils
{
  template<typename T, size_t MAX_ITEMS>
  class CFixedList
  {
  public:
    template<typename ...Args>
    T* CreateItem(Args&&... args)
    {
      T*& pItem = m_vctItemList[m_uRegisteredItems++];
      pItem = new T(std::forward<Args>(args)...);
      return pItem;
    }
    bool RemoveItem(T* _pItem_);
    void ClearAll();

    const uint32_t& CurrentSize() const { return m_uRegisteredItems; }
    uint32_t MaxSize() { return static_cast<uint32_t>(MAX_ITEMS); }

    std::array<T*, MAX_ITEMS>& operator()() { return m_vctItemList; }
    T* operator[](size_t Index) { return m_vctItemList[Index]; }

  private:
    std::array<T*, MAX_ITEMS> m_vctItemList = std::array<T*, MAX_ITEMS>();
    uint32_t m_uRegisteredItems = 0;
  };

  template<typename T, size_t MAX_ITEMS>
  void CFixedList<T, MAX_ITEMS>::ClearAll()
  {
    std::for_each(m_vctItemList.begin(), m_vctItemList.end(), [](T* _pItem)
    {
      if (_pItem)
      {
        delete _pItem;
        _pItem = nullptr;
      }
    });
    m_uRegisteredItems = 0;
  }

  template<typename T, size_t MAX_ITEMS>
  bool CFixedList<T, MAX_ITEMS>::RemoveItem(T* _pItem_)
  {
    auto it = std::find(m_vctItemList.begin(), m_vctItemList.end(), _pItem_);
    if (it != m_vctItemList.end())
    {
      delete* it;
      *it = nullptr;
      m_uRegisteredItems--;

      auto oReorderFunc = std::remove_if(m_vctItemList.begin(), m_vctItemList.end(),
        [](T* _pPtr) { return _pPtr == nullptr; }); // Reorder fixed list
      std::fill(oReorderFunc, m_vctItemList.end(), nullptr); // Set nullptr

      return true;
    }
    return false;
  }

}


