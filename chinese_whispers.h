// Copyright (C) 2012  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_CHINESE_WHISPErS_Hh_
#define DLIB_CHINESE_WHISPErS_Hh_

#include <vector>
#include <map>
#include "sample_pair.h"
#include "ordered_sample_pair.h"
#include <algorithm>
#include <ctime>
#include <assert.h>
#include "config.h"
#define DLIB_ASSERT(p1, p2)

#if USING_STL
#define ARRAY std::vector
#define MAP std::map
#else
#define ARRAY Array
#define MAP Map
#endif

#define SAFE_DELETE_ARRAY(p) if (p) { delete [] (p); (p) = NULL; }

namespace dlib
{
// ----------------------------------------------------------------------------------------
	struct rand 
	{
		rand() { srand(time(NULL)); }
		int get_random_64bit_number() const { return ::rand(); }
	};

	/* Array - 替代vector */
	template <typename T>
	class Array
	{
	private:
		unsigned m_capacity;		// 最大元素数
		unsigned m_size;			// 当前元素数
		T *data;					// 数据存储区
	public:
		Array(unsigned s = 0) : m_capacity(s), m_size(0), data(s ? new T[s] : 0) { }
		~Array() { clear(); }
		unsigned size() const { return m_size; }
		unsigned capacity() const { return m_capacity; }
		void clear() { SAFE_DELETE_ARRAY(data); memset(this, 0, sizeof(*this)); }
		void reserve(unsigned s) { if(s > m_capacity) change(s); }// 预分配内存
		const T& operator[](unsigned index) const { return *(data + index); }
		T& operator[](unsigned index) { return *(data + index); }
		T* begin() const { return data; }
		T* end() const { return data + m_size; }
		void push_back(const T &e){if(m_size==m_capacity) change(1.5*m_capacity); *(data+m_size)=e; ++m_size;}
		void resize(unsigned s) { if(s < m_size) m_size = s; else if(s > m_capacity){change(s); m_size = s;} }
		void assign(unsigned s, const T &e) { change(1.5 * s); for(int i = 0; i < s; ++i) push_back(e); }
	private:
		void change(unsigned s)// 改变数组容量
		{
			T *buf = new T[m_capacity = s];
			m_size = m_size < m_capacity ? m_size : m_capacity;
			if (data) memcpy(buf, data, m_size * sizeof(T));
			SAFE_DELETE_ARRAY(data);
			data = buf;
		}
	};

	/* Map - 替代map */
	template <typename Idx, typename Val>
	class Map
	{
	private:
		unsigned m_capacity;			// 最大索引号
		std::pair<Idx, Val> *data;		// 索引与值映射表
	public:
		Map(unsigned s = 0) : m_capacity(s), data(s ? new std::pair<Idx, Val>[s] : 0)
		{
			for (int i = 0; i < m_capacity; ++i)
				(data + i)->first = i;
		}
		~Map() { SAFE_DELETE_ARRAY(data); }
		void reserve(unsigned s) { if(s > m_capacity) change(s); }// 预分配内存
		const Val& operator[](Idx index) const { if(index>=m_capacity) change(1.5*(index+1)); return (data + index)->second; }
		Val& operator[](Idx index) { if(index>=m_capacity) change(1.5*(index+1)); return (data + index)->second; }

		typedef typename std::pair<Idx, Val>* iterator;
		iterator begin() const { return data; }
		iterator end() const { return data + m_capacity; }
	private:
		void change(unsigned s)// 改变数组容量
		{
			std::pair<Idx, Val> *buf = new std::pair<Idx, Val>[s];
			if (data) memcpy(buf, data, m_capacity * sizeof(std::pair<Idx, Val>));
			for (int i = m_capacity; i < s; ++i) (buf + i)->first = i;
			SAFE_DELETE_ARRAY(data);
			data = buf;
			m_capacity = s;
		}
	};

// max_index_plus_one, find_neighbor_ranges 来自"dlib/edge_list_graphs.h"

// ----------------------------------------------------------------------------------------

	inline int max_index_plus_one(const ARRAY<ordered_sample_pair>& pairs)
	{
		if (pairs.size() == 0)
		{
			return 0;
		}
		else
		{
			unsigned long max_idx = 0;
			for (unsigned long i = 0; i < pairs.size(); ++i)
			{
				if (pairs[i].index1() > max_idx)
					max_idx = pairs[i].index1();
				if (pairs[i].index2() > max_idx)
					max_idx = pairs[i].index2();
			}

			return max_idx + 1;
		}
	}

// ----------------------------------------------------------------------------------------

	inline void find_neighbor_ranges (
	const ARRAY<ordered_sample_pair>& edges,
	ARRAY<std::pair<unsigned long, unsigned long>>& neighbors
	)
	{
		// make sure requires clause is not broken
		DLIB_ASSERT(is_ordered_by_index(edges),
			"\t void find_neighbor_ranges()"
			<< "\n\t Invalid inputs were given to this function"
			);


		// setup neighbors so that [neighbors[i].first, neighbors[i].second) is the range
		// within edges that contains all node i's edges.
		const unsigned long num_nodes = max_index_plus_one(edges);
		neighbors.assign(num_nodes, std::make_pair(0,0));
		unsigned long cur_node = 0;
		unsigned long start_idx = 0;
		for (unsigned long i = 0; i < edges.size(); ++i)
		{
			if (edges[i].index1() != cur_node)
			{
				neighbors[cur_node] = std::make_pair(start_idx, i);
				start_idx = i;
				cur_node = edges[i].index1();
			}
		}
		if (neighbors.size() != 0)
			neighbors[cur_node] = std::make_pair(start_idx, (unsigned long)edges.size());
	}

// ----------------------------------------------------------------------------------------

	inline void convert_unordered_to_ordered (
	const ARRAY<sample_pair>& edges,
	ARRAY<ordered_sample_pair>& out_edges
	)
	{
		out_edges.clear();
		out_edges.reserve(edges.size()*2);
		for (unsigned long i = 0; i < edges.size(); ++i)
		{
			out_edges.push_back(ordered_sample_pair(edges[i].index1(), edges[i].index2(), edges[i].distance()));
			if (edges[i].index1() != edges[i].index2())
				out_edges.push_back(ordered_sample_pair(edges[i].index2(), edges[i].index1(), edges[i].distance()));
		}
	}

// 以下代码来自"dlib/chinese_whispers.h"

// ----------------------------------------------------------------------------------------

    inline unsigned long chinese_whispers (
        const ARRAY<ordered_sample_pair>& edges,
        ARRAY<unsigned long>& labels,
        const unsigned long num_iterations,
        dlib::rand& rnd
    )
    {
        // make sure requires clause is not broken
        DLIB_ASSERT(is_ordered_by_index(edges),
                    "\t unsigned long chinese_whispers()"
                    << "\n\t Invalid inputs were given to this function"
        );

        labels.clear();
        if (edges.size() == 0)
            return 0;

        ARRAY<std::pair<unsigned long, unsigned long> > neighbors;
        find_neighbor_ranges(edges, neighbors);

        // Initialize the labels, each node gets a different label.
        labels.resize(neighbors.size());
        for (unsigned long i = 0; i < labels.size(); ++i)
            labels[i] = i;


        for (unsigned long iter = 0; iter < neighbors.size()*num_iterations; ++iter)
        {
            // Pick a random node.
            const unsigned long idx = rnd.get_random_64bit_number()%neighbors.size();

            // Count how many times each label happens amongst our neighbors.
            MAP<unsigned long, double> labels_to_counts;
#if !USING_STL
			labels_to_counts.reserve(labels.size());
#endif
            const unsigned long end = neighbors[idx].second;
            for (unsigned long i = neighbors[idx].first; i != end; ++i)
            {
                labels_to_counts[labels[edges[i].index2()]] += edges[i].distance();
            }

            // find the most common label
            MAP<unsigned long, double>::iterator i;
            double best_score = -std::numeric_limits<double>::infinity();
            unsigned long best_label = labels[idx];
            for (i = labels_to_counts.begin(); i != labels_to_counts.end(); ++i)
            {
                if (i->second > best_score)
                {
                    best_score = i->second;
                    best_label = i->first;
                }
            }

            labels[idx] = best_label;
        }


        // Remap the labels into a contiguous range.  First we find the
        // mapping.
        std::map<unsigned long,unsigned long> label_remap;
        for (unsigned long i = 0; i < labels.size(); ++i)
        {
            const unsigned long next_id = label_remap.size();
            if (label_remap.count(labels[i]) == 0)
                label_remap[labels[i]] = next_id;
        }
        // now apply the mapping to all the labels.
        for (unsigned long i = 0; i < labels.size(); ++i)
        {
            labels[i] = label_remap[labels[i]];
        }

        return label_remap.size();
    }

// ----------------------------------------------------------------------------------------

    inline unsigned long chinese_whispers (
        const ARRAY<sample_pair>& edges,
        ARRAY<unsigned long>& labels,
        const unsigned long num_iterations,
        dlib::rand& rnd
    )
    {
        ARRAY<ordered_sample_pair> oedges;
        convert_unordered_to_ordered(edges, oedges);
        std::sort(oedges.begin(), oedges.end(), &order_by_index<ordered_sample_pair>);

        return chinese_whispers(oedges, labels, num_iterations, rnd);
    }

// ----------------------------------------------------------------------------------------

    inline unsigned long chinese_whispers (
        const ARRAY<sample_pair>& edges,
        ARRAY<unsigned long>& labels,
        const unsigned long num_iterations = 100
    )
    {
        dlib::rand rnd;
        return chinese_whispers(edges, labels, num_iterations, rnd);
    }

// ----------------------------------------------------------------------------------------

    inline unsigned long chinese_whispers (
        const ARRAY<ordered_sample_pair>& edges,
        ARRAY<unsigned long>& labels,
        const unsigned long num_iterations = 100
    )
    {
        dlib::rand rnd;
        return chinese_whispers(edges, labels, num_iterations, rnd);
    }

// ----------------------------------------------------------------------------------------

}

#endif // DLIB_CHINESE_WHISPErS_Hh_
