#ifndef SJTU_BPLUSTREE_HPP
#define SJTU_BPLUSTREE_HPP

#include <cstddef>
#include <functional>
#include <stdio.h>
#include <cstring>
#include "exceptions.h"
#include "alloc.hpp"
using pointer = long;
using byte = char;
const pointer invalid_p = 0xdeadbeef;
template<class key_type,
	class value_type,
	size_t part_size,
	class Compare = std::less<key_type>
>   class bplustree {
	struct node {
		key_type key;
		pointer prior, next;
		pointer pos;
		size_t size;                    //the size of its brothers
		bool type;                      //0 for a leaf and 1 otherwise
		node(key_type k = key_type(),
			pointer p = invalid_p,
			pointer pre = invalid_p, pointer nex = invalid_p,
			size_t s = 1, bool ty = 0)
			:key(k), pos(p), prior(pre), next(nex), size(s), type(ty) {}
	};
	node root;
	Compare com;
	size_t num;
	FILE *datafile, *bptfile;
	const size_t node_size;
	const size_t inf_size;
	ALLOC alloc, alloc_data;
	pointer root_pos;
	char *dfa_name, *ba_name, *b_name, *df_name;

	inline bool equal(const key_type& k1, const key_type& k2) {
		return !(com(k1, k2) || com(k2, k1));
	}
	inline void load_cache_n(byte *start, const node& p) {
		fseek(bptfile, p.pos + sizeof(node), SEEK_SET);
		// printf("load_cache_seek: %d\n", p.pos + sizeof(node));
		fread(start, 1, (sizeof(key_type) + sizeof(pointer)) * p.size, bptfile);
		// printf("which are:\n");
		// for (int i = 0;i < p.size;i++) // printf("key: %d, pointer: %d; ", *nth_key(start, i), *nth_pointer(start, i));
		// printf("\n");
	}
	inline void load_cache_l(byte *start, const node& p) {
		fseek(bptfile, p.pos + sizeof(node), SEEK_SET);
		// printf("load_cache_seek: %d\n", p.pos + sizeof(node));
		fread(start, 1, (sizeof(key_type) + sizeof(value_type)) * p.size, bptfile);
		// printf("which are:\n");
		// for (int i = 0;i < p.size;i++) // printf("key: %d, pointer: %d; ", *nth_key(start, i), *nth_pointer(start, i));
		// printf("\n");
	}
	inline void save_cache_n(byte *start, const node &p) {
		fseek(bptfile, p.pos + sizeof(node), SEEK_SET);
		// printf("save_cache_seek: %d\n", p.pos + sizeof(node));
		fwrite(start, 1, (sizeof(key_type) + sizeof(pointer)) * p.size, bptfile);
		// printf("which are:\n");
		// for (int i = 0;i < p.size;i++)// printf("key: %d, pointer: %d; ", *nth_key(start, i), *nth_pointer(start, i));
		// printf("\n");
	}
	inline void save_cache_l(byte *start, const node &p) {
		fseek(bptfile, p.pos + sizeof(node), SEEK_SET);
		// printf("save_cache_seek: %d\n", p.pos + sizeof(node));
		fwrite(start, 1, (sizeof(key_type) + sizeof(value_type)) * p.size, bptfile);
		// printf("which are:\n");
		// for (int i = 0;i < p.size;i++)// printf("key: %d, pointer: %d; ", *nth_key(start, i), *nth_pointer(start, i));
		// printf("\n");
	}
	inline node load_node(pointer l) {
		fseek(bptfile, l, SEEK_SET);
		// printf("load_node_seek: %d\n", l);
		node tmp;
		fread(&tmp, sizeof(node), 1, bptfile);
		// printf("which is: pos:%d size:%d key:%d\n", tmp.pos, tmp.size, tmp.key);
		return tmp;
	}
	inline bool save_node(const node &p) {
		if (p.pos == invalid_p) return false;
		try {
			fseek(bptfile, p.pos, SEEK_SET);
			// printf("save_node_seek: %d\n", p.pos);
			fwrite(&p, sizeof(node), 1, bptfile);
			// printf("which is: pos:%d size:%d key:%d\n", p.pos, p.size, p.key);
			return true;
		}
		catch (...) {
			return false;
		}
	}
	inline key_type* nth_key_n(byte *start, size_t n = 0) {
		return (key_type *)(start + (sizeof(key_type) + sizeof(pointer)) * n);
	}
	inline key_type* nth_key_l(byte *start, size_t n = 0) {
		return (key_type *)(start + (sizeof(key_type) + sizeof(value_type)) * n);
	}
	inline pointer* nth_pointer(byte *start, size_t n = 0) {
		return (pointer *)(start + sizeof(key_type) * (n + 1) + sizeof(pointer) * n);
	}
	inline pointer nth_value_loc(const node &now, size_t n = 0) {
		return now.pos + sizeof(node) + (sizeof(key_type) + sizeof(value_type)) * n + sizeof(key_type);
	}
	inline value_type get_value(pointer loc) {
		value_type v;
		fseek(bptfile, loc, SEEK_SET);
		fread(&v, sizeof(value_type), 1, bptfile);
		return v;
	}
	inline size_t binary_search_key_n(byte *start, const key_type& k, size_t n) {
		size_t l = 0, r = n, mid;
		while (l < r) {
			mid = (l + r) / 2;
			if (com(*nth_key_n(start, mid), k)) l = mid + 1;
			else r = mid;
		}
		if (l >= n) return n - 1;
		else if (equal(*nth_key_n(start, l), k)) return l;
		else return l - 1;
	}
	inline size_t binary_search_key_l(byte *start, const key_type& k, size_t n) {
		size_t l = 0, r = n, mid;
		while (l < r) {
			mid = (l + r) / 2;
			if (com(*nth_key_l(start, mid), k)) l = mid + 1;
			else r = mid;
		}
		if (l >= n) return n - 1;
		else if (equal(*nth_key_l(start, l), k)) return l;
		else return l - 1;
	}
	pointer _find(const node &p, const key_type& k) {
		byte cache[inf_size];
		size_t ord;
		if (p.type) {
			load_cache_n(cache, p);
			ord = binary_search_key_n(cache, k, p.size);
			pointer tmp = *nth_pointer(cache, ord);
			return _find(load_node(tmp), k);
		}
		else {
			load_cache_l(cache, p);
			ord = binary_search_key_l(cache, k, p.size);
			if (equal(*nth_key(cache, ord), k)) return nth_value_loc(p, ord);
			else return invalid_p;
		}
	}
	/*
		* make the first of p become the last of l while the size of p equals to part_size and that of l is less.
		* save them both
	*/
	inline void left_balance(node &p, node &l, byte *p_cache) {
		byte left_cache[inf_size];
		load_cache(left_cache, l);
		*nth_key(left_cache, l.size) = p.key;
		*nth_pointer(left_cache, l.size) = *nth_pointer(p_cache);
		++l.size, --p.size;
		p.key = *nth_key(p_cache, 1);
		save_cache(left_cache, l);
		save_cache((byte *)nth_key(p_cache, 1), p);
		save_node(l), save_node(p);
	}
	/*
		* Make the last of p become the first of r while the size of p equals to part_size and that of r is less.
		* Save them both. do we need so?
	*/
	inline void right_balance(node &p, node &r, byte *p_cache) {
		byte right_cache[inf_size];
		load_cache(right_cache, r);
		for (size_t i = r.size; i > 0; i--) {
			*nth_key(right_cache, i) = *nth_key(right_cache, i - 1);
			*nth_pointer(right_cache, i) = *nth_pointer(right_cache, i - 1);
		}
		++r.size; --p.size;
		*nth_key(right_cache, 0) = *nth_key(p_cache, p.size);
		*nth_pointer(right_cache) = *nth_pointer(p_cache, p.size);
		r.key = *nth_key(right_cache, 0);
		save_cache(right_cache, r);
		save_cache(p_cache, p);
		save_node(r), save_node(p);
	}
	/*
		* Receive a node from the left part if avaliable
		* Save them both
	*/
	inline void receive_left(node &p, node &l, byte *p_cache) {
		byte left_cache[inf_size];
		load_cache(left_cache, l);
		for (size_t i = p.size; i > 0; i--) {
			*nth_key(p_cache, i) = *nth_key(p_cache, i - 1);
			*nth_pointer(p_cache, i) = *nth_pointer(p_cache, i - 1);
		}
		p.key = *nth_key(p_cache, 0) = *nth_key(left_cache, l.size);
		*nth_pointer(p_cache) = *nth_pointer(left_cache, l.size);
		++p.size; --l.size;
		save_cache(left_cache, l);
		save_cache(p_cache, p);
		save_node(l), save_node(p);
	}
	/*
		* Receive a node from the right part if avaliable.
		* Save them both
	*/
	inline void receive_right(node &p, node &r, byte *p_cache) {
		byte right_cache[inf_size];
		load_cache(right_cache, r);
		*nth_key(p_cache, p.size) = *nth_key(right_cache, 0);
		*nth_pointer(p_cache, p.size) = *nth_pointer(right_cache);
		++p.size, --r.size;
		r.key = *nth_key(right_cache, 1);
		save_cache((byte *)nth_key(right_cache, 1), r);
		save_cache(p_cache, p);
		save_node(r), save_node(p);
	}
	/*
		* Find the quickest way to solve the problem of extreme size.
		* save now and cache_now and left/right and its cache if necessary,
		* but do not save the cache of the parent
	*/
	inline void deal_surplus(node &now, node &par, byte *cache, byte *cache_par, size_t ord) {
		node tmp;
		if (now.pos != *nth_pointer(cache_par)) {
			tmp = load_node(now.prior);
			if (tmp.size < part_size - 1) {
				left_balance(now, tmp, cache);
				*nth_key(cache_par, ord) = now.key;
				return;
			}
		}
		if (now.pos != *nth_pointer(cache_par, par.size - 1)) {
			tmp = load_node(now.next);
			if (tmp.size < part_size - 1) {
				right_balance(now, tmp, cache);
				*nth_key(cache_par, ord + 1) = tmp.key;
				return;
			}
			else split(now, cache, par, cache_par, ord);
		}
		else split(now, cache, par, cache_par, ord);
	}
	inline void deal_deficit(node &now, node &par, byte *cache, byte *cache_par, size_t ord) {
		node tmp;
		if (now.pos != *nth_pointer(cache_par, par.size - 1)) {
			tmp = load_node(now.next);
			if (tmp.size >= part_size / 2) {
				receive_right(now, tmp, cache);
				*nth_key(cache_par, ord + 1) = tmp.key;
				return;
			}
			else merge(now, cache, par, cache_par, tmp);
		}
		if (now.pos != *nth_pointer(cache_par)) {
			tmp = load_node(now.prior);
			if (tmp.size >= part_size / 2) {
				receive_left(now, tmp, cache);
				*nth_key(cache_par, ord) = now.key;
				return;
			}
			else {
				byte cache_tmp[inf_size]; load_cache(cache_tmp, tmp);
				merge(tmp, cache_tmp, par, cache_par, now);
			}
		}
	}
	/*
		* split the cache belonging to node now.
		* save now and its cache but do not save any info of parent into storage,
		* though we have changed it in memory
	*/
	void split(node &now, byte *cache, node &par, byte *cache_par, size_t order) {
		size_t s = now.size / 2;
		now.size -= s;
		size_t ns = now.size;
		pointer pos = alloc.alloc(node_size);
		// printf("ask_for_bpt: %d where %d\n", node_size, pos);
		node tmp = node(*nth_key(cache, ns), pos, now.pos, now.next, s, now.type);
		if (now.next != invalid_p) {
			node temp = load_node(now.next);
			temp.prior = tmp.pos;
			save_node(temp);
		}
		now.next = tmp.pos;
		byte *cache_tmp = (byte *)nth_key(cache, ns);
		save_cache(cache_tmp, tmp);
		save_cache(cache, now);
		ns = order;
		for (size_t i = par.size; i > ns + 1; i--) {
			*nth_key(cache_par, i) = *nth_key(cache_par, i - 1);
			*nth_pointer(cache_par, i) = *nth_pointer(cache_par, i - 1);
		}
		*nth_key(cache_par, ns + 1) = tmp.key;
		*nth_pointer(cache_par, ns + 1) = tmp.pos;
		++par.size;
		save_node(now); save_node(tmp);
	}
	//merge now and the next of it, if the result is too big, use split automatically
	void merge(node &now, byte *cache, node &par, byte *cache_par, node &tmp) {
		now.next = tmp.next;
		if (tmp.next != invalid_p) {
			node temp = load_node(tmp.next);
			temp.prior = now.pos;
			save_node(temp);
		}
		size_t s = now.size;
		load_cache(cache + now.size, tmp.size);
		now.size += tmp.size;
		alloc.free(tmp.pos, node_size);
		s = binary_search_key(cache_par, now.key, par.size);
		--par.size;
		for (size_t i = s + 1; i < par.size; i++) {
			*nth_key(cache_par, i) = *nth_key(cache_par, i + 1);
			*nth_pointer(cache_par, i) = *nth_pointer(cache_par, i + 1);
		}
		if (now.size >= part_size) split(now, cache, par, cache_par);
		save_cache(cache, now);
		save_node(now);
	}
	/*
		* Insert (k,v) and return true if it is an insertion and false for a change,
		* whether split or not is considered in its parent,
		* as we prove that all parts are smaller than part_size, the memory is safe.
		* But mention to judge the size of the root.(actually, it does need to be specially treated)
	*/
	bool _insert(node &now, const key_type &k, const value_type &v, byte *cache) {
		size_t ord = binary_search_key(cache, k, now.size);
		pointer loc = *nth_pointer(cache, ord);
		if (now.type) {
			node child_node = load_node(loc);
			byte cache_child[inf_size];
			load_cache(cache_child, child_node);
			bool ret = _insert(child_node, k, v, cache_child);
			if (child_node.size >= part_size) {
				deal_surplus(child_node, now, cache_child, cache, ord);
			}
			else {
				save_node(child_node);
				save_cache(cache_child, child_node);
			}
			return ret;
		}
		else {
			if (equal(*nth_key(cache, ord), k)) return false;
			++num;
			pointer pos = alloc_data.alloc(sizeof(key_type) + sizeof(value_type));
			// printf("ask_for_data: at %d\n", pos);
			fseek(datafile, pos, SEEK_SET);
			// printf("_insert_new_value_seek: %d\n", pos);
			fwrite(&k, sizeof(key_type), 1, datafile);
			fwrite(&v, sizeof(value_type), 1, datafile);
			// printf("which is: key: %d value: %d\n", k, v);
			for (size_t i = now.size; i > ord + 1; i--) {
				*nth_key(cache, i) = *nth_key(cache, i - 1);
				*nth_pointer(cache, i) = *nth_pointer(cache, i - 1);
			}
			++now.size;
			*nth_key(cache, ord + 1) = k;
			*nth_pointer(cache, ord + 1) = pos;
			// p.key = *nth_key(p, 0); shall we add this?
			return true;
		}
	}
	void _insert_head(node &now, const key_type &k, const value_type &v, byte *cache) {
		if (now.type) {
			node node_child = load_node(*nth_pointer(cache));
			byte cache_child[inf_size]; load_cache(cache_child, node_child);
			_insert_head(node_child, k, v, cache_child);
			*nth_key(cache, 0) = now.key = k;
			if (node_child.size >= part_size) {
				deal_surplus(node_child, now, cache_child, cache, 0);
			}
			else {
				save_node(node_child);
				save_cache(cache_child, node_child);
			}
			return;
		}
		else {
			++num;
			pointer pos = alloc_data.alloc(sizeof(key_type) + sizeof(value_type));
			// printf("ask_for_data: at %d\n", pos);
			fseek(datafile, pos, SEEK_SET);
			// printf("_insert_new_value_seek: %d\n", pos);
			fwrite(&k, sizeof(key_type), 1, datafile);
			fwrite(&v, sizeof(value_type), 1, datafile);
			for (size_t i = now.size; i > 0; i--) {
				*nth_key(cache, i) = *nth_key(cache, i - 1);
				*nth_pointer(cache, i) = *nth_pointer(cache, i - 1);
			}
			++now.size;
			*nth_key(cache, 0) = now.key = k;
			*nth_pointer(cache, 0) = pos;
		}
	}
	bool _remove(node &p, const key_type &k, byte *cache = nullptr) {
		if (cache != nullptr) {
			cache = new byte[inf_size];
			load_cache(cache, p);
		}
		size_t ord = binary_search_key(cache, k, p.size);
		if (p.type) {
			pointer loc = *nth_pointer(cache, ord);
			node child_node = load_node(loc);
			byte cache_child[inf_size];
			bool ret = _remove(child_node, k);
			p.key = *nth_key(cache, 0);
			if (child_node.size < part_size / 2) {
				deal_deficit(child_node, p, cache, ord);
			}
			save_node(child_node);
			return ret;
		}
		else {
			if (*nth_key(cache, ord) == k) {
				alloc_data.free(*nth_pointer(cache, ord), sizeof(key_type) + sizeof(pointer));
				--num;
				--p.size;
				for (size_t i = ord; i < p.size; i++) {
					*nth_key(cache, i) = *nth_key(cache, i + 1);
					*nth_pointer(cache, i) = *nth_pointer(cache, i + 1);
				}
				save_cache(cache, p);
				p.key = *nth_key(cache, 0);
				return true;
			}
			else return false;
			//or throw something?
		}
	}
public:
	bplustree() :node_size(sizeof(node) + (sizeof(key_type) + sizeof(pointer)) * part_size),
		inf_size((sizeof(key_type) + sizeof(pointer)) * part_size),
		root(), num(0), root_pos(0) {}
	void initialize(const char *datafile_name, const char *bptfile_name, const char *data_alloc, const char *bpt_alloc) {
		ba_name = new char[strlen(bpt_alloc) + 1];
		strcpy(ba_name, bpt_alloc);
		dfa_name = new char[strlen(data_alloc) + 1];
		strcpy(dfa_name, data_alloc);
		b_name = new char[strlen(bptfile_name) + 1];
		strcpy(b_name, bptfile_name);
		df_name = new char[strlen(datafile_name) + 1];
		strcpy(df_name, datafile_name);
		alloc.initialize(ba_name);
		alloc_data.initialize(dfa_name);
		bptfile = fopen(bptfile_name, "rb+");
		if (!bptfile) bptfile = fopen(bptfile_name, "wb+");
		datafile = fopen(datafile_name, "rb+");
		if (!datafile) datafile = fopen(datafile_name, "wb+");
		fseek(bptfile, 0, SEEK_SET);
		// printf("size_of_node: %d node_size_with_cache: %d\n", sizeof(node), node_size);
		// printf("ini_root_pointer_seek: %d\n", 0);
		if (!fread(&root_pos, sizeof(pointer), 1, bptfile)) {
			alloc.alloc(sizeof(pointer));
			root_pos = alloc.alloc(node_size);
			fseek(bptfile, root_pos, SEEK_SET);
			// printf("ini_root_seek: %d\n", root_pos);
			fwrite(&root, sizeof(node), 1, bptfile);
			// printf("which is: pos:%d size:%d key:%d\n", root.pos, root.size, root.key);
		}
		// printf("\n%d\n", root_pos);
	}
	void clear() {
		if (bptfile) fclose(bptfile);
		if (datafile) fclose(datafile);
		bptfile = fopen(b_name, "wb+");
		datafile = fopen(df_name, "wb+");
		delete b_name; delete df_name;
		alloc.refill(ba_name);
		alloc_data.refill(dfa_name);
		fseek(bptfile, 0, SEEK_SET);
		num = 0;
		// printf("clear_root_pointer_seek: %d\n", 0);
		if (!fread(&root_pos, sizeof(pointer), 1, bptfile)) {
			alloc.alloc(sizeof(pointer));
			root_pos = alloc.alloc(node_size);
			fseek(bptfile, root_pos, SEEK_SET);
			// printf("clear_root_seek: %d\n", 0);
			fwrite(&root, sizeof(node), 1, bptfile);
			// printf("which is: pos:%d size:%d key:%d\n", root.pos, root.size, root.key);
		}
	}
	bool empty() {
		return !num;
	}
	~bplustree() {
		alloc.save(ba_name);
		alloc_data.save(dfa_name);
		save_node(root);
		fseek(bptfile, 0, SEEK_SET);
		if (root.pos != invalid_p) root_pos = root.pos;
		// printf("decode_root_pointer_seek: %d\nwhich is: %d\n", 0, root_pos);
		fwrite(&root_pos, sizeof(pointer), 1, bptfile);
		// printf("which is: %d\n", root_pos);
		if (bptfile) fclose(bptfile);
		if (datafile) fclose(datafile);
		delete ba_name; delete dfa_name; delete b_name; delete df_name;
	}
	int count(const key_type &k) const {
		if (empty()) return 0;
		pointer p = _find(root, k);
		if (p == invalid_p) return 0;
		return 1;
	}
	value_type find(const key_type &k) const {
		if (empty()) throw(container_is_empty());
		pointer p = _find(root, k);
		if (p == invalid_p) return value_type();
		fseek(datafile, p + sizeof(key_type), SEEK_SET);
		value_type v;
		fread(&v, sizeof(value_type), 1, datafile);
		// printf("find_seek_in_database: %d\nwhich is: %d\n", p + sizeof(key_type),v);
		return v;
	}
	bool set(const key_type &k, const value_type &v) {
		if (empty()) throw(container_is_empty());
		pointer p = _find(root, k);
		if (p == invalid_p) return 0;
		fseek(datafile, p + sizeof(key_type), SEEK_SET);
		fwrite(&v, sizeof(value_type), 1, datafile);
		return 1;
	}
	bool insert(const key_type &k, const value_type &v) {
		if (root.pos == invalid_p) {
			if (!bptfile) throw(runtime_error());
			root.pos = root_pos;
			root.size = num = 1; root.key = k; root.type = 0;
			byte cache[inf_size];
			load_cache(cache, root);
			*nth_key(cache, 0) = k;
			*nth_pointer(cache) = alloc_data.alloc(sizeof(key_type) + sizeof(value_type));
			fseek(datafile, *nth_pointer(cache), SEEK_SET);
			// printf("insert_no_root_seek: %d\n", *nth_pointer(cache));
			fwrite(&k, sizeof(key_type), 1, datafile); fwrite(&v, sizeof(value_type), 1, datafile);
			// printf("which is: key: %d value: %d\n", k, v);
			save_cache(cache, root);
			return true;
		}
		bool ret = true;
		byte cache[inf_size]; load_cache(cache, root);
		if (com(root.key, k))
			ret = _insert(root, k, v, cache);
		else {
			_insert_head(root, k, v, cache);
		}
		save_cache(cache, root);
		//a save_node(root) is needed after this
		if (root.size >= part_size) {
			pointer pos = alloc.alloc(node_size);
			// printf("ask_for_bpt: at %d\n", pos);
			node now_root(root.key, pos);
			now_root.type = 1, now_root.size = 1;
			size_t s = root.size / 2;
			root.size -= s;
			root.next = pos = alloc.alloc(node_size);
			// printf("ask_for_bpt: at %d\n", pos);
			node temp(*nth_key(cache, root.size), pos, root.pos);
			temp.type = root.type, temp.size = s;
			save_cache((byte *)nth_key(cache, root.size), temp); save_node(temp);
			pos = now_root.pos;
			fseek(bptfile, pos + sizeof(node), SEEK_SET);
			// printf("extend_root_seek: %d\n", pos + sizeof(node));
			fwrite(&root.key, sizeof(key_type), 1, bptfile);
			fwrite(&root.pos, sizeof(pointer), 1, bptfile);
			fwrite(&temp.key, sizeof(key_type), 1, bptfile);
			fwrite(&temp.pos, sizeof(pointer), 1, bptfile);
			// printf("which are: first_key: %d first_pos: %d second_key: %d second_pos: %d\n", root.key, root.pos, temp.key, temp.pos);
			save_node(root);
			root = now_root; root_pos = root.pos; root.size = 2;
		}
		else save_cache(cache, root);
		return ret;
	}
	bool remove(key_type k) {
		return true;
	}
	void listof(key_type k, bool(*comp)(key_type a, key_type b)) {
		if (empty()) throw(container_is_empty());
		pointer p = _find(root, k);
	}
	int size() const { return num; }
};
#endif