
namespace selfdestructs {

	struct no_feedback {
		void ping(int copy_nr) {
		}
	};

	template <int N,typename TFeedback=no_feedback>
	struct on {
		class copies {
			int copy_nr;
			TFeedback feedback;
		public:
			
			copies():copy_nr(0) {
				feedback.ping(copy_nr);
			}

			copies(const copies& other)
			{
				copy_nr=other.copy_nr+1;
				feedback.ping(copy_nr);
			}

			copies& operator=(const copies& other)  {
				copies tmp(other);
				copy_nr=tmp.copy_nr;
				return *this;
			}



		};
		typedef copies copy;
	};
}