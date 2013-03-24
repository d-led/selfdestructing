#include <stdexcept>
#include <memory>
#include <functional>
#include <algorithm>

namespace crashes {

	typedef std::function<void (int)> on_feedback;

	namespace detail {

		//////////////////////////////////////////////
		typedef std::shared_ptr<size_t> shared_number;

		////////////////////
		class simple_count {
			size_t copy_nr;
		public:

			simple_count():copy_nr(0){}

			size_t get_copy_nr() const {
				return copy_nr;
			}

			void increment() {
				++copy_nr;
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

			shared_count():copy_nr(new size_t(0)){}

			size_t get_copy_nr() const {
				return *copy_nr;
			}

			void increment() {
				++(*copy_nr);
			}

			bool should_have_crashed_on(size_t) {
				return false;
			}
		};

		////////////////////////////////////////
		struct should_decrement_on_destruction {
			static const bool should_decrement=true;
		};

		//////////////////////////////////////////
		struct shouldnt_decrement_on_destruction {
			static const bool should_decrement=false;
		};

		/////////////////////
		template <typename T,typename TDecrement>
		class total_count
		{
			static size_t count_nr;

		public:

			total_count()
			{
				increment();
			}

			size_t get_copy_nr() const {
				return count_nr;
			}

			void increment() {
				++count_nr;
			}

			bool should_have_crashed_on(size_t N) {
				return count_nr>=N;
			}

			//protected:

			~total_count()
			{
				if (TDecrement::should_decrement)
					--count_nr;
			}
		};
		template <typename T,typename TDecrement> size_t total_count<T,TDecrement>::count_nr(0);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		template <size_t MaxN,typename TFeedback=on_feedback, typename TCounter=simple_count>
		struct when {
			class copies {
				TCounter counter;
				TFeedback feedback;
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
					counter.increment();
					size_t count=counter.get_copy_nr();
					if (feedback)
						feedback(count);
					if (count>=MaxN)
						throw std::runtime_error("illegal number of copies");
				}

				copies& operator=(copies const& other)  {
					copies tmp(other);
					swap(other);
					return *this;
				}

				void swap(copies const& other) {
					std::swap(counter,other.counter);
					std::swap(feedback,other.feedback);
				}

			public:

				void set_feedback(TFeedback const& f) {
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