#include <exception>
#include <memory>
#include <functional>
#include <algorithm>

namespace crashes {

	typedef std::function<void (int)> on_feedback;
	typedef std::shared_ptr<int> shared_int;

	template <int MaxN,typename TFeedback=on_feedback, typename TSharedInt=shared_int>
	struct on {
		class copies {
			TSharedInt copy_nr;
			TFeedback feedback;
		public:
			
			copies():copy_nr(new int(0)) { }

			copies(const copies& other):
				copy_nr(other.copy_nr),
				feedback(other.feedback)
			{
				++(*copy_nr);
				if (feedback)
					feedback(*copy_nr);
				if (*copy_nr>=MaxN)
					throw std::runtime_error("illegal number of copies");
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

	template <int MaxN,typename TFeedback=on_feedback>
	struct after {
		class copies {
			std::shared_ptr<int> copy_nr;
			TFeedback feedback;
		public:
			
			copies():copy_nr(new int(0)) {
				feedback.ping(*copy_nr);
			}

			copies(const copies& other):
				copy_nr(other.copy_nr),
				feedback(other.feedback)
			{
				++(*copy_nr);
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