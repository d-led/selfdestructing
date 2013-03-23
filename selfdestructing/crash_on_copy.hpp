#include <exception>
#include <memory>
#include <functional>
#include <algorithm>

namespace crashes {

	typedef std::function<void (int)> on_feedback;

	namespace detail {

		////////////////////////////////////////
		typedef std::shared_ptr<size_t> shared_number;
		
		/////////////////////////////////////////
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
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	template <int MaxN,typename TFeedback=on_feedback, typename TCounter=detail::shared_count<detail::shared_number>>
	struct on {
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

	template <int MaxN,typename TFeedback=on_feedback>
	struct after {
		class copies {
			int copy_nr;
			TFeedback feedback;
		public:
			
			copies():copy_nr(0) { }

			copies(const copies& other):
				copy_nr(other.copy_nr+1),
				feedback(other.feedback)
			{
				if (feedback)
					feedback(copy_nr);
				if (copy_nr>=MaxN)
					throw std::runtime_error("illegal copy");
			}

			copies& operator=(copies const& other)  {
				copies tmp(other);
				swap(other);
				return *this;
			}

			void swap(copies const& other) {
				std::swap(copy_nr,other.copy_nr);
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