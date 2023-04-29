#include "reassembler.hh"
#include <iostream>
#include <stack>
#include <algorithm> 


using namespace std;

Intervals::Intervals()
  : arr(), size_(0)
{}

bool compareInterval(Interval i1, Interval i2)
{
    return (i1.start < i2.start);
}

void Intervals::insertInterval(Interval in) {

  auto itr = std::lower_bound(arr.begin(), arr.end(), in, compareInterval);
  arr.insert(itr, in);
  size_ += in.data.length();
  // cout << "size ++ " << size_ << " \n";

  // std::sort(arr.begin(), arr.end(), compareInterval);

  stack<Interval> s;
  s.push(arr.at(0));

  for (unsigned int i = 1; i < arr.size(); i++) {
    Interval top = s.top();
    // cout << "top " << top.start << " " << top.end << " arr " << arr.at(i).start << " " << arr.at(i).end << "\n";
    if (top.end < arr.at(i).start) {
      s.push(arr.at(i));
    } else if (top.end < arr.at(i).end) {
      // cout << "substr 1 " << top.start << "  " <<  top.end << "  " << arr.at(i).start << "  " << arr.at(i).end << "\n";
      top.data += arr.at(i).data.substr(top.end - arr.at(i).start, arr.at(i).data.length() - top.end + arr.at(i).start + 1);
      // cout << "substr 2 " << top.end - arr.at(i).start << "  " << arr.at(i).data.length() - top.end + arr.at(i).start + 1 << "\n";
      size_ -= top.end - arr.at(i).start;
      top.end = arr.at(i).end;
      // cout << "data concatenated is " << top.data << "\n";
      // cout << "size -- " << size_ << " \n";
      s.pop();
      s.push(top);
    } else {
      size_ -= arr.at(i).data.length();
      // cout << "size -- " << size_ << " \n";
    }
  }
  arr.clear();
  while (!s.empty()) {
    arr.insert(arr.begin(), s.top());
    s.pop();
  }
}

Interval Intervals::top() const {
  return arr[0];
}

void Intervals::pop() {
  size_ -= arr.front().data.length();
  arr.erase(arr.begin());
}

bool Intervals::empty() const {
  return arr.size() == 0;
}

uint64_t Intervals::size() const {
  return size_;
}

void Intervals::print() {
  // cout << "priting current intervals " << size_ << "\n";
  for (unsigned int i = 0; i < arr.size(); i++) {
    cout << i << "   " << arr.at(i).start << "  " << arr.at(i).end << "  " << arr.at(i).data << " \n";
  }
}

Reassembler::Reassembler()
  : index( 0 ), last_substring_index(-1), index_intervals()
{}

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring, Writer& output )
{

  // cout << "==============================\n";

  // cout << "pushing start index  " << first_index << " length " << data.length() << "string " << data << "\n";
  // cout << "first_index " << first_index << " last string index " << last_substring_index << "\n";

  if (is_last_substring) {
    // cout << "last substring \n";
    last_substring_index = first_index;
  }

  if (index + output.available_capacity() <= first_index) return;

  if (index >= first_index) {
    if (first_index + data.length() <= index) {
      if (is_last_substring) {
        output.close();
      }
      return;
    }
    uint64_t start_index = 0;
    uint64_t end_index = 0;

    start_index = index - first_index;
    end_index = min(data.length(), start_index + output.available_capacity()); 

    // cout << "start " << start_index << " end " << end_index << "\n";

    output.push(data.substr(start_index, end_index-start_index)); 
    index += end_index - start_index;

    if (int(first_index) == last_substring_index) {
      output.close();
    }

    while((!index_intervals.empty()) && index_intervals.top().start <= index) {
      Interval top = index_intervals.top();
      index_intervals.pop();
      insert(top.start, top.data, false, output);
      // index_intervals.print();
    }

  } else {
    uint64_t end_index = min(first_index + data.length(), index + output.available_capacity()); 

    // cout << "start " << first_index << " end " << end_index << "\n";

    index_intervals.insertInterval({first_index, end_index, data.substr(0, end_index - first_index)});
    // index_intervals.print();
  }
  
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return index_intervals.size();
}

uint64_t Reassembler::current_index()
{
  return index;
}