#include <exception>
#include <memory>
#include <functional>
#include <algorithm>

namespace crashes {

	typedef std::function<void (int)> on_feedback;

	namespace detail {

		//////////////////////////////////////////////
		typedef std::shared_ptr<size_t> shared_number;

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
		};

		////////////////////////////////////////////////////////////////////////////////////////////////////
		template <size_t MaxN,typename TFeedback=on_feedback, typename TCounter=shared_count<shared_number>>
		struct when {
			class copies {
				TCounter counter;
				TFeedback feedback;
			public:

				copies() { }

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

	template <size_t MaxN>
	struct on : public detail::when<MaxN,on_feedback,detail::shared_count<detail::shared_number>>
	{};

	template <size_t MaxN>
	struct after : public detail::when<MaxN,on_feedback,detail::simple_count>
	{};
}