#pragma once

#include <vector>
#include <unordered_map>

namespace Ubpa {
	template<typename T>
	class random_set {
	public:
		void insert(T* e) {
			assert(!contains(e));
			Tmap.emplace(e, Tvec.size());
			Tvec.push_back(e);
		}

		void erase(T* e) {
			auto target = Tmap.find(e);
			assert(target != Tmap.end());
			auto eIdx = target->second;
			if (eIdx != Tvec.size() - 1) {
				Tmap[Tvec.back()] = eIdx;
				Tvec[eIdx] = Tvec.back();
			}
			Tvec.pop_back();
			Tmap.erase(target);
		}

		auto begin() noexcept { return Tvec.begin(); }
		auto begin() const noexcept { return Tvec.begin(); }
		auto end() noexcept { return Tvec.end(); }
		auto end() const noexcept { return Tvec.end(); }

		T* operator[](std::size_t i) const noexcept { return Tvec[i]; }

		T* at(std::size_t i) const { return Tvec.at(i); }

		std::size_t size() const noexcept { return Tvec.size(); }

		void reserve(std::size_t n) {
			Tvec.reserve(n);
			Tmap.reserve(n);
		}
		void clear() noexcept {
			Tvec.clear();
			Tmap.clear();
		}

		const std::vector<T*>& vec() const noexcept { return Tvec; }

		std::size_t idx(T* e) const { return Tmap.at(e); }

		bool contains(T* e) const { return Tmap.find(e) != Tmap.end(); }

		bool empty() const noexcept { return Tvec.empty(); }
	private:
		std::unordered_map<T*, std::size_t> Tmap;
		std::vector<T*> Tvec;
	};
}
