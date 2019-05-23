class Main {
  main(): Int {
    let
      x:Bool <- true,
      x:Bool <- not x
    in
      if x then 1 else 0 fi
  };
};
