
// Will be extended as the need arose
template<typename T, T* (T::*GetNext)() const, void (T::*SetNext)(T*)>
class CustomList {
public:
    CustomList() = default;
    explicit CustomList(T* head) : head_(head) {}

    bool Empty() const {
        return head_ == nullptr;
    }

    void Clear() {
        head_ = nullptr;
    }

    T& Front() {
        ASSERT(head_ != nullptr);
        return *head_;
    }

    void PushFront(T& item) {
        (item.*SetNext)(head_);
        head_ = &item;
    }

    void PopFront() {
        ASSERT(head_ != nullptr);
        head_ = GetNextHelper(head_);
    }

    void EraseAfter(T* prev, T* current)
    {
        if (current == head_) {
            head_ = GetNextHelper(current);
        } else {
            SetNextHelper(prev, GetNextHelper(current));
        }
    }

    template <typename Predicate>
    bool RemoveIf(Predicate pred)
    {
        bool found = false;
        auto prev = head_;
        for (auto current = head_; current != nullptr; current = GetNextHelper(current)) {
            if (pred(*current)) {
                found = true;
                EraseAfter(prev, current);
                current = prev;
            } else {
                prev = current;
            }
        }
        return found;
    }

    void Splice(CustomList& other) {
        if (Empty()) {
            head_ = other.head_;
        } else {
            T *last = head_;
            for (; last->GetNextWait() != nullptr; last = GetNextHelper(last)) {}
            SetNextHelper(last, other.head_);
        }
        other.Clear();
    }

private:

    ALWAYS_INLINE T* GetNextHelper(T* item) const {
        return ((*item).*GetNext)();
    }

    ALWAYS_INLINE void SetNextHelper(T* prev, T* item) {
        ((*prev).*SetNext)(item);
    }
private:
    T* head_{nullptr};
};
