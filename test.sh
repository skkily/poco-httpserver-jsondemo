curl -X POST -H "Content-Type: application/json" -d '{"pos_min": 0, "pos_max": -1}' 127.0.0.1:8001/search
echo
curl -X POST -H "Content-Type: application/json" -d '{"pos": [1, 2, 3]}' 127.0.0.1:8001/admit
echo
curl -X POST -H "Content-Type: application/json" -d '{}' 127.0.0.1:8001/list
echo
curl -X POST -H "Content-Type: application/json" -d '{"pos": [0, 1, 2], "course": "math"}' 127.0.0.1:8001/course
echo
