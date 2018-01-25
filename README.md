# :sparkles::sparkles:**PedestrianCounter-C++** :sparkles::sparkles:
This is a C++ reimplementation and refinement of [blacksteed232's **PedestrianCounter**](https://github.com/blacksteed232/PedestrianCounter) with trakcking in mind. 

## Requirements
- [x] Opencv
## Improvements
* Instead of counting how many people walking across a pre-defined line, here we can specify any closed polygon and count the number of people entering or exiting from it. 

* We eliminate the constraint that people can only come inside or go outside of scene from the top and bottom edges, and allow entering and exiting from any location on the boudary.
* When perfoming cv::Camshift, we give two options, namely tracking by gray scale value or tracking by hue value. As you can see, tracking by hue value is more stable than the other approach.
* New data members `momentOnTrack`, which is the miliseconds offset from the beginning of the input video and records the moment a person starts on track, and `trackings`, which is of type `std::vector<cv::Point2i>>` and keeps the trojectories of each individual, are added to `class People`. Just before deleting each `People` object, you can store their tracking info into whatever databases you like.
* We use double-linked list data structure to dynamically organize the list of `People` objects. Unlike removing objects directly from a Python list in [**PedestrianCounter**](https://github.com/blacksteed232/PedestrianCounter), this approach is undoubtly more efficient. 

## Examples
We only correctly identify the trojectories of 3 out of 4 persons ever accross our predefined region.

<img src="https://user-images.githubusercontent.com/26423200/35379054-c459b77c-01ef-11e8-8f0a-a8a158f3ea03.jpg" width="250"><img src="https://user-images.githubusercontent.com/26423200/35379195-46d82972-01f0-11e8-8f0d-04a9a3d837f8.jpg" width="250">
<img src="https://user-images.githubusercontent.com/26423200/35379222-5b77d44a-01f0-11e8-967a-06f1e2732dff.jpg" width="250">

<img src="https://user-images.githubusercontent.com/26423200/35379205-4cd6b6c2-01f0-11e8-8ac3-4d26ab70c605.jpg" width="250"><img src="https://user-images.githubusercontent.com/26423200/35379219-5847769a-01f0-11e8-977a-1ce2652153ad.jpg" width="250"><img src="https://user-images.githubusercontent.com/26423200/35379225-5dec7a32-01f0-11e8-822e-4dc25cb71709.jpg" width="250">

### Command Line Results

![screenshot from 2018-01-25 19-01-13](https://user-images.githubusercontent.com/26423200/35385074-287bd0d4-0202-11e8-807b-37ab7b512836.png)

As you can see, we identify 10 persons in total. The number on the right of each output log is the area of the bounding box when a person leaves the scene.
