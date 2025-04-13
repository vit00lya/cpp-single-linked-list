#pragma once

#include <algorithm>
#include <vector>
#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <forward_list>
#include <iostream>
#include <initializer_list>

template<typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type &val, Node *next) :
                value(val), next_node(next) {
        }
        Type value;
        Node *next_node = nullptr;
    };

    template<typename ValueType>
    class BasicIterator {
        // Класс списка объявляется дружественным, чтобы из методов списка
        // был доступ к приватной области итератора
        friend class SingleLinkedList;

        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node *node) {
            node_ = node;
        }

    public:
        // Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора

        // Категория итератора — forward iterator
        // (итератор, который поддерживает операции инкремента и многократное разыменование)
        using iterator_category = std::forward_iterator_tag;
        // Тип элементов, по которым перемещается итератор
        using value_type = Type;
        // Тип, используемый для хранения смещения между итераторами
        using difference_type = std::ptrdiff_t;
        // Тип указателя на итерируемое значение
        using pointer = ValueType*;
        // Тип ссылки на итерируемое значение
        using reference = ValueType&;

        BasicIterator() = default;

        // Конвертирующий конструктор/конструктор копирования
        // При ValueType, совпадающем с Type, играет роль копирующего конструктора
        // При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
        BasicIterator(const BasicIterator<Type> &other) noexcept {
            node_ = other.node_;
        }

        // Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
        // пользовательского конструктора копирования, явно объявим оператор = и
        // попросим компилятор сгенерировать его за нас
        BasicIterator& operator=(const BasicIterator &rhs) = default;

        // Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
        [[nodiscard]] bool operator==(
                const BasicIterator<const Type> &rhs) const noexcept {
            return (rhs.node_ == node_);
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]] bool operator!=(
                const BasicIterator<const Type> &rhs) const noexcept {
            return !(rhs == *this);
        }

        // Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
        [[nodiscard]] bool operator==(
                BasicIterator<const Type> &rhs) const noexcept {
            return (rhs.node_ == node_);
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]] bool operator!=(
                BasicIterator<const Type> &rhs) const noexcept {
            return !(rhs == *this);
        }

        // Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает ссылку на самого себя
        // Инкремент итератора, не указывающего на существующий элемент списка, приводит к неопределённому поведению
        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }

        // Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает прежнее значение итератора
        // Инкремент итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        BasicIterator operator++(int) noexcept {
            auto old_value(*this); // Сохраняем прежнее значение объекта для последующего возврата
            ++(*this); // используем логику префиксной формы инкремента
            return old_value;
        }

        // Операция разыменования. Возвращает ссылку на текущий элемент
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }

        // Операция доступа к члену класса. Возвращает указатель на текущий элемент списка
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &node_->value;
        }

    private:
        Node *node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;

    // Возвращает итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    [[nodiscard]] Iterator begin() noexcept {
        return Iterator { head_.next_node };
    }

    // Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator end() noexcept {
        return Iterator { nullptr };
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    // Результат вызова эквивалентен вызову метода cbegin()
    [[nodiscard]] ConstIterator begin() const noexcept {
        return cbegin();
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    // Результат вызова эквивалентен вызову метода cend()
    [[nodiscard]] ConstIterator end() const noexcept {
        return cend();
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен cend()
    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator(head_.next_node);
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator(nullptr);
    }

    // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator before_begin() noexcept {
        // Реализуйте самостоятельно
        return Iterator(&head_);
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        // Реализуйте самостоятельно
        return ConstIterator(const_cast<Node*>(&head_));
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        // Реализуйте самостоятельно
        return ConstIterator(const_cast<Node*>(&head_));
    }

public:

    SingleLinkedList() {
    }

    SingleLinkedList(std::initializer_list<Type> values) {
        PushListTmp(values.begin(), values.end());
    }

    SingleLinkedList(const SingleLinkedList &other) {
        // Сначала надо удостовериться, что текущий список пуст
        if (other.IsEmpty())
            return;
        PushListTmp(other.begin(), other.end());
        // Теперь tmp пуст, а текущий список содержит копию элементов other
    }

    void SetSize(size_t size) {
        size_ = size;
    }

    Iterator InsertAfter(ConstIterator pos, const Type &value) {
        assert(pos.node_ != nullptr);
        Node *new_node = new Node(value, pos.node_->next_node);
        pos.node_->next_node = new_node;
        size_++;
        return Iterator { pos.node_->next_node };
    }

    void PopFront() noexcept {
        if (IsEmpty())
            return;
        auto del_pointer = head_.next_node;
        head_.next_node = del_pointer->next_node;
        delete del_pointer;
        --size_;
    }

    /*
     * Удаляет элемент, следующий за pos.
     * Возвращает итератор на элемент, следующий за удалённым
     */
    Iterator EraseAfter(ConstIterator pos) noexcept {
        if (IsEmpty())
            return Iterator { nullptr };
        assert(pos.node_ != nullptr);
        assert(pos.node_->next_node != nullptr);
        if (pos.node_ == nullptr) {
            return Iterator { nullptr };
        } else {
            Node *delete_node = pos.node_->next_node;
            pos.node_->next_node = pos.node_->next_node->next_node;
            size_--;
            delete delete_node;

            return Iterator { pos.node_->next_node };
        }
    }

    SingleLinkedList& operator=(const SingleLinkedList &rhs) {
        assert(head_.next_node != rhs.head_.next_node);
        if (this != &rhs) {
            SingleLinkedList tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    void swap(SingleLinkedList &other) noexcept {

        if (other.IsEmpty())
            return;

        std::swap(other.size_, this->size_);
        std::swap(other.head_.next_node, this->head_.next_node);

    }

    // Возвращает количество элементов в списке
    [[nodiscard]] size_t GetSize() const noexcept {
        return this->size_;
    }

    // Вставляет элемент value в начало списка за время O(1)
    void PushFront(const Type &value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    // Очищает список за время O(N)
    void Clear() noexcept {
        if (size_ == 0)
            return;
        while (head_.next_node != nullptr) {
            auto node_to_delete = head_.next_node;
            head_.next_node = head_.next_node->next_node;
            delete node_to_delete;
        }
        size_ = 0;
    }

    void InitHead(const Type &value){
        head_.next_node = new Node(value, nullptr);
        ++size_;
    }

    ~SingleLinkedList() {
        Clear();
    }

    // Сообщает, пустой ли список
    [[nodiscard]] bool IsEmpty() const noexcept {
        return (GetSize() == 0);
    }

private:
    template<typename It>
    void PushListTmp(const It &begin, const It &end) {
        auto p = begin;
        SingleLinkedList tmp;
        tmp.InitHead(*p);
        auto tmp_p = tmp.begin();
        std::advance(p, 1);
        while (p != end) {
            tmp_p = tmp.InsertAfter(tmp_p, *p);
            std::advance(p, 1);
        }
        swap(tmp);
    }

    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_;
    size_t size_ = 0;
};

template<typename Type>
bool operator==(const SingleLinkedList<Type> &lhs,
        const SingleLinkedList<Type> &rhs) {
    if (lhs.begin() == rhs.begin())
        return true;
    return (lhs.GetSize() == rhs.GetSize()
            && std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
}

template<typename Type>
bool operator!=(const SingleLinkedList<Type> &lhs,
        const SingleLinkedList<Type> &rhs) {
    return !(lhs == rhs);
}

template<typename Type>
bool operator<(const SingleLinkedList<Type> &lhs,
        const SingleLinkedList<Type> &rhs) {
    return (std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
            rhs.end()));
}

template<typename Type>
bool operator>(const SingleLinkedList<Type> &lhs,
        const SingleLinkedList<Type> &rhs) {
    return rhs < lhs;
}

template<typename Type>
bool operator<=(const SingleLinkedList<Type> &lhs,
        const SingleLinkedList<Type> &rhs) {
    return !(rhs < lhs);
}

template<typename Type>
bool operator>=(const SingleLinkedList<Type> &lhs,
        const SingleLinkedList<Type> &rhs) {
    return !(rhs > lhs);
}

template<typename Type>
void swap(SingleLinkedList<Type> &lhs, SingleLinkedList<Type> &rhs) noexcept {
    lhs.swap(rhs);
}
