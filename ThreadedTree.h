#ifndef THREADEDTREE_H
#define THREADEDTREE_H
using namespace std;
template <class T>
class ThreadedTree
{
	struct Node
	{
		T data_;
		Node* left_;
		Node* right_;
		bool leftIsThread_;
		bool rightIsThread_;
		Node(const T& data = T{}, Node* left = nullptr, Node* right = nullptr, bool leftIsThread = false, bool rightIsThread = false) {
			data_ = data;
			left_ = left;
			right_ = right;
			leftIsThread_ = leftIsThread;
			rightIsThread_ = rightIsThread;
		}
	};
	
	Node* head_=nullptr;      // head nodes, the left node is root node
	Node* begin_ = nullptr;   // the most left node
	unsigned int amount_=0;   // the amount of the nodes


public:

	class const_iterator
	{
	protected:
		friend class ThreadedTree;
		Node* curr_ = nullptr;    // current node
		const ThreadedTree* myThreadedTree_ = nullptr;
	public:
		// iterator constructor
		const_iterator(Node* curr = nullptr,const ThreadedTree* tree=nullptr) {
			curr_ = curr;
			myThreadedTree_ = tree;
		}

		// postfix ++, const_iterator advances to next biggest value in the tree if the const_iterator is not currently at end().
		// the function returns const_iterator to node before the increment.
		const_iterator operator++(int) {
			const_iterator temp = *this;
			if (curr_) {
				if(curr_->rightIsThread_)
					curr_ = curr_->right_;
				else {
					curr_ = curr_->right_;
					while(!curr_->leftIsThread_)
						curr_ = curr_->left_;
				}
			}	
			return temp;
		}

		// postfix --, const_iterator advances to next smallest value in the tree if the const_iterator is not currently at end().
		// the function returns const_iterator to node before the decrement.
		const_iterator operator--(int) {
			const_iterator temp = *this;
			if (curr_) {
				if (curr_->leftIsThread_)
					curr_ = curr_->left_;
				else {
					curr_ = curr_->left_;
					while (!curr_->rightIsThread_)
						curr_ = curr_->right_;
				}
			}
			return temp;
		}

		// prefix ++, const_iterator advances to next biggest value in the tree if the const_iterator is not currently at end().
		// the function returns const_iterator to current node.
		const_iterator operator++() {
			if (curr_) {
				if (curr_->rightIsThread_)
					curr_ = curr_->right_;
				else {
					curr_ = curr_->right_;
					while (!curr_->leftIsThread_)
						curr_ = curr_->left_;
				}
			}
			return *this;
		}

		// prefix --, const_iterator advances to next smallest value in the tree if the const_iterator is not currently at end().
		// the function returns const_iterator to current node.
		const_iterator operator--() {
			if (curr_) {
				if (curr_->leftIsThread_)
					curr_ = curr_->left_;
				else {
					curr_ = curr_->left_;
					while (!curr_->rightIsThread_)
						curr_ = curr_->right_;
				}
			}
			return *this;
		}

		//returns a const reference to data in the node referred to by the iterator.
		const T& operator*() const {
			return curr_->data_;
		}

		// function returns true if rhs and current object refer to the same node.
		bool operator==(const const_iterator& rhs) const {
			return curr_ == rhs.curr_;
		}

		// function returns true if rhs and current object does not refer to the same node.
		bool operator!=(const const_iterator& rhs) const {
			return curr_ != rhs.curr_;
		}
	};

	class iterator :public const_iterator{
	public:
		iterator() :const_iterator() {}

		iterator(Node* curr, ThreadedTree* tree) :const_iterator(curr, tree) {};

		//returns a const reference to data in the node referred to by the iterator.
		const T& operator*() const {
			return this->const_iterator::operator*();
		}

		T& operator*() {
			return const_cast<T&>(this->const_iterator::operator*());
		}

		// postfix ++, iterator advances to next biggest value in the tree if the iterator is not currently at end().
		// the function returns iterator to node before the increment.
		iterator operator++(int) {
			iterator temp = *this;
			this->const_iterator::operator++();
			return temp;
		}

		// postfix --, iterator advances to next smallest value in the tree if the iterator is not currently at end().
		// the function returns iterator to node before the decrement.
		iterator operator--(int) {
			iterator temp = *this;
			this->const_iterator::operator--();
			return temp;
		}

		// prefix ++, iterator advances to next biggest value in the tree if the iterator is not currently at end().
		// the function returns iterator to current node.
		iterator operator++() {
			this->const_iterator::operator++();
			return *this;
		}

		// prefix --, iterator advances to next smallest value in the tree if the iterator is not currently at end().
		// the function returns iterator to current node.
		iterator operator--() {
			this->const_iterator::operator--();
			return *this;
		}
	};

	// constructor, creates empty ThreadedTree.
	ThreadedTree() {
		head_ = new Node();
		head_->left_ = head_;
		head_->right_ = head_;
		amount_ = 0;
		begin_ = head_;
	}

	// The function inserts the data into the tree.
	void insert(const T& data) {
		if (!amount_) {
			head_->left_ =new Node(data, head_, head_, true, true);
			amount_++;
			begin_ = head_->left_;
		}	
		else{
			bool isLastLine = false;
			Node* temp = head_->left_;
			while (!isLastLine) {
				if (data < temp->data_) {
					if (temp->leftIsThread_) {
						temp->left_ = new Node(data, temp->left_, temp, true, true);
						amount_++;
						temp->leftIsThread_ = false;
						isLastLine = true;
						if (temp == begin_)
							begin_ = temp->left_;
					}
					else
						temp = temp->left_;
				}
				else if (temp->data_ < data) {
					if (temp->rightIsThread_) {
						temp->right_ = new Node(data, temp, temp->right_, true, true);
						amount_++;
						temp->rightIsThread_ = false;
						isLastLine = true;
					}
					else
						temp = temp->right_;
				}
			}
		}
	}

	// this function accepts a value, key and returns an iterator/const_iterator to the node containing that value. 
	// If key is not found, function returns end()/cend().
	iterator find(const T& key) {
		bool isFound = false;
		Node* temp = head_->left_;
		if (amount_) {
			bool isLastline = false;
			while (!isFound && !isLastline) {
				if (temp->data_ == key)
					isFound = true;
				else if (key < temp->data_) {
					if (temp->leftIsThread_)
						isLastline = true;
					else
						temp = temp->left_;
				}
				else {
					if(temp->rightIsThread_)
						isLastline = true;
					else
						temp = temp->right_;
				}
			}
		}
		if (isFound)
			return iterator(temp, this);
		else
			return this->end();
	}

	// this function accepts a value, key and returns an iterator/const_iterator to the node containing that value. 
	// If key is not found, function returns end()/cend().
	const_iterator find(const T& key) const {
		bool isFound = false;
		Node* temp = head_->left_;
		if (amount_) {
			bool isLastline = false;
			while (!isFound && !isLastline) {
				if (temp->data_ == key)
					isFound = true;
				else if (key < temp->data_) {
					if (temp->leftIsThread_)
						isLastline = true;
					else
						temp = temp->left_;
				}
				else {
					if (temp->rightIsThread_)
						isLastline = true;
					else
						temp = temp->right_;
				}
			}
		}
		if (isFound)
			return const_iterator(temp, this);
		else
			return this->cend();
	}

	// returns the appropriate iterator to the smallest node in the tree.
	iterator begin() {
		return iterator(begin_,this);
	}

	// returns the appropriate iterator to the node after the largest node in the tree.
	iterator end() {
		return iterator(head_, this);
	}

	// returns the appropriate iterator to the smallest node in the tree.
	const_iterator cbegin()const {
		return const_iterator(begin_, this);
	}

	// returns the appropriate iterator to the node after the largest node in the tree.
	const_iterator cend() const {
		return const_iterator(head_, this);
	}

	// This function returns the number of values stored in the tree.
	int size() const {
		return amount_;
	}

	// This function returns true if the tree has no values stored, false otherwise
	bool empty() const {
		return amount_ == 0;
	}

	// destructor for ThreadedTree. All nodes are deallocated.
	~ThreadedTree() {
		const_iterator iter = this->cbegin();
		while (iter != this->cend()) {
			Node* temp = iter.curr_;
			++iter;
			delete temp;
			temp = nullptr;
		}
		delete head_;
		head_ = nullptr;
	}
};
#endif