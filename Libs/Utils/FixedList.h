#pragma once
#include <algorithm>
#include <array>
#include <iostream>

namespace utils
{
  template<typename T, size_t MAX_ITEMS>
  class CFixedList
  {
  public:
    template<typename Inheritance = T, typename ...Args>
    T* CreateItem(Args&&... args)
    {
      if (m_uRegisteredItems >= m_vctItemList.size())
      {
        std::cout << "Maximum items reached!" << std::endl;
        return nullptr;
      }
      Inheritance* pItem = new Inheritance(std::forward<Args>(args)...);
      m_vctItemList[m_uRegisteredItems++] = pItem;
      return pItem;
    }
    bool RemoveItem(T* _pItem_);
    void ClearAll();

    const uint32_t& CurrentSize() const { return m_uRegisteredItems; }
    uint32_t GetMaxSize() { return static_cast<uint32_t>(MAX_ITEMS); }

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
    if (it != m_vctItemList.end() && *it)
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


