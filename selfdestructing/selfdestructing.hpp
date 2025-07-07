#include <stdexcept>
#include <memory>
#include <functional>
#include <algorithm>
#include <atomic>
#include <mutex>

namespace crashes {

typedef std::function<void (int)> on_feedback;

	namespace detail {

		//////////////////////////////////////////////
		typedef std::shared_ptr<std::atomic<size_t>> shared_number;

		////////////////////
		class simple_count {
			std::atomic<size_t> copy_nr;
		public:

			simple_count():copy_nr(0){}
			
			simple_count(const simple_count& other):copy_nr(other.copy_nr.load()){}

			size_t increment() {
				return copy_nr.fetch_add(1) + 1;
			}

			bool should_have_crashed_on(size_t) {
				return false;
			}
		};

		////////////////////////////////////////////
		template <typename TSharedInt=shared_number>
		class shared_count {
			TSharedInt copy_nr;

		public:

			shared_count():copy_nr(std::make_shared<std::atomic<size_t>>(0)){}

			size_t increment() {
				return copy_nr->fetch_add(1) + 1;
			}

			bool should_have_crashed_on(size_t) {
				return false;
			}
		};

		////////////////////////////////////////
		struct should_decrement_on_destruction {
			static std::atomic<bool> should_decrement;
		};

		//////////////////////////////////////////
		struct shouldnt_decrement_on_destruction {
			static std::atomic<bool> should_decrement;
		};

		/////////////////////
		template <typename T,typename TDecrement>
		class total_count
		{
			static std::atomic<size_t> count_nr;

		public:

			total_count()
			{
				increment();
			}

			size_t increment() {
				return count_nr.fetch_add(1) + 1;
			}

			bool should_have_crashed_on(size_t N) {
				return count_nr.load()>=N;
			}

			//protected:

			~total_count()
			{
				if (TDecrement::should_decrement)
					count_nr.fetch_sub(1);
			}
		};
		template <typename T,typename TDecrement> std::atomic<size_t> total_count<T,TDecrement>::count_nr(0);
        
        std::atomic<bool> should_decrement_on_destruction::should_decrement(true);
        std::atomic<bool> shouldnt_decrement_on_destruction::should_decrement(false);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		template <size_t MaxN,typename TFeedback=on_feedback, typename TCounter=simple_count>
		struct when {
			class copies {
				TCounter counter;
				TFeedback feedback;
				mutable std::mutex mutex_;
			public:

				copies() {
					if (counter.should_have_crashed_on(MaxN))
						throw std::runtime_error("illegal number of copies");
				}

				virtual ~copies() {} //todo: review

				copies(const copies& other):
					counter(other.counter),
					feedback(other.feedback)
				{
					std::lock_guard<std::mutex> lock(mutex_);
					const size_t count=counter.increment();
					if (feedback)
						feedback(count);
					if (count>=MaxN)
						throw std::runtime_error("illegal number of copies");
				}

				copies& operator=(copies const& other)  {
					if (this != &other) {
						std::lock(mutex_, other.mutex_);
						std::lock_guard<std::mutex> lock1(mutex_, std::adopt_lock);
						std::lock_guard<std::mutex> lock2(other.mutex_, std::adopt_lock);
						counter = other.counter;
						feedback = other.feedback;
					}
					return *this;
				}

				void swap(copies const& other) {
					if (this != &other) {
						std::lock(mutex_, other.mutex_);
						std::lock_guard<std::mutex> lock1(mutex_, std::adopt_lock);
						std::lock_guard<std::mutex> lock2(other.mutex_, std::adopt_lock);
						std::swap(counter,other.counter);
						std::swap(feedback,other.feedback);
					}
				}

			public:

				void set_feedback(TFeedback const& f) {
					std::lock_guard<std::mutex> lock(mutex_);
					feedback=f;
				}

			};
			typedef copies copy;
		};
	}

	/// crashes on MaxN total number of copies
	template <size_t MaxN>
	struct on : public detail::when<MaxN,on_feedback,detail::shared_count<detail::shared_number>>
	{};

	/// crashes on MaxN'th copy
	template <size_t MaxN>
	struct after : public detail::when<MaxN,on_feedback,detail::simple_count>
	{};

	/// crashes on MaxN total instances of T
	template <size_t MaxN,typename T>
	struct on_total : public detail::when<MaxN,on_feedback,detail::total_count<T,detail::should_decrement_on_destruction>>
	{
		// instead typedef  copies instance - for gcc/standard complience
		typedef typename detail::when<MaxN, on_feedback, detail::total_count<T, detail::should_decrement_on_destruction> >::copies instances;
		typedef typename detail::when<MaxN, on_feedback, detail::total_count<T, detail::should_decrement_on_destruction> >::copies instance;
	};

	/// crashes on MaxN total instances of T
	template <size_t MaxN,typename T>
	struct after_total : public detail::when<MaxN,on_feedback,detail::total_count<T,detail::shouldnt_decrement_on_destruction>>
	{
		typedef typename detail::when<MaxN, on_feedback, detail::total_count<T, detail::shouldnt_decrement_on_destruction> >::copies instances;
		typedef typename detail::when<MaxN, on_feedback, detail::total_count<T, detail::shouldnt_decrement_on_destruction> >::copies instance;
	};
}